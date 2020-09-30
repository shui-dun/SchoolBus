#include "Card.h"
#include "DBConnector.h"
#include <iostream>
#include <cstdlib>
using std::cout;
using std::cin;
using std::to_string;

bool Card::chk_bus_status_(Bus& bus) {
    if (bus.status()) {
        cout << "很抱歉，校车已经开走了，请等待下一辆校车。\n";
    } else if (bus.is_full()) {
        cout << "很抱歉，校车已满载\n";
    } else if (bus.contains_passenger(id_)) {
        cout << "您已上车，请勿重复刷卡\n";
    } else {
        n_ride_++;
        sync();
        bus.insert_passenger(id_);
        if (bus.is_full()) {
            bus.set_status(true);
        }
        cout << "欢迎乘车！\n";
        return true;
    }
    return false;
}

bool Card::logout() {
    string ch;
    cout << "你确定要注销账号吗，注销后您将不能乘坐校车。确定注销请输入y，返回请输入n\n";
    cin >> ch;
    if (ch == "y" || ch == "yes") {
        DBConnector().exec("DELETE FROM " + db_tbl_ + " WHERE id=" + to_string(id_) + ";");
        cout << "注销成功。\n";
        return true;
    } else {
        return false;
    }
}

void Card::show_info() {
    cout << "您好：" << name_ << '\n';
    cout << "ID：" << id_ << '\n';
    cout << "本月乘车次数：" << n_ride_ << '\n';
}

void Card::chk_int_input(int& data, int min, int max) {
    std::string s;
    bool success = false;
    while (true) {
        cin >> s;
        try {
            data = stoi(s);
            if (data<min || data>max) {
                cout << "请输入[" << min << ',' << max << "]之间的数。\n";
                continue;
            }
            return;
        } catch (const std::invalid_argument&) {
            cout << "请输入数字。\n";
        }
    }
}

bool Card::exist() {
    return !DBConnector().load("SELECT id FROM " + db_tbl_ + " WHERE id=" + to_string(id_) + ";").empty();
}

void Card::logout_entry(Card*& card) {
    if (card->logout()) {
        delete card;
        card = nullptr;
    }
}

void Card::login(Card*& card) {
    if (card) {
        delete card;
        card = nullptr;
        cout << "已退出登录当前账号。\n";
    }
    int type;
    cout << "请输入你的身份：(0代表教师，1代表学生，2代表教师家属)\t";
    chk_int_input(type, 0, 2);
    if (type == 0) {
        card = new Teacher();
    } else if (type == 1) {
        card = new Student();
    } else {
        card = new FamMember();
    }
    cout << "请输入你的ID\t";
    chk_int_input(card->id_, 0, 100000);
    if (card->load_info()) {
        cout << "登陆成功\n";
        card->show_info();
    } else {
        delete card;
        card = nullptr;
        cout << "登陆失败\n";
    }
}

void Card::sign_up_entry(Card*& card) {
    int type;
    cout << "欢迎注册一卡通！\n";
    cout << "请输入你的身份：(0代表教师，1代表学生，2代表教师家属)\t";
    chk_int_input(type, 0, 2);
    if (type == 0) {
        card = new Teacher();
    } else if (type == 1) {
        card = new Student();
    } else {
        card = new FamMember();
    }
    if (!card->sign_up_()) {
        delete card;
        card = nullptr;
    }
}

void Student::show_info() {
    Card::show_info();
    cout << "学院：" << school_ << '\n';
    cout << "余额：" << balance_ << '\n';
}

void Student::ride(Bus& bus) {
    if (check_balance_()) {
        chk_bus_status_(bus);
    }
    show_info();
}

bool Student::load_info() {
    auto vec = DBConnector().load("SELECT * FROM " + db_tbl_ + " WHERE id=" + to_string(id_) + ";");
    if (vec.empty()) {
        return false;
    } else {
        name_ = vec[1];
        gender_ = (Card::Gender)stoi(vec[2]);
        school_ = vec[3];
        balance_ = stoi(vec[4]);
        n_ride_ = stoi(vec[5]);
        return true;
    }
}

void Student::sync() {
DBConnector().exec("UPDATE " + db_tbl_ + " SET name=\"" + name_ + "\",gender=\"" + to_string((int)gender_) + "\",school=\"" + school_ + "\",balance=" + to_string(balance_) + ",n_ride=" + to_string(n_ride_) + " WHERE id=" + to_string(id_) + ";");
}

bool Student::sign_up_() {
    cout << "请输入你的学号：\t";
    chk_int_input(id_, 0, 100000);
    if (exist()) {
        cout << "该账号已经被注册，注册失败！\n";
        return false;
    } else {
        cout << "请输入你所在的学院：\t";
        cin >> school_;
        cout << "请输入你的姓名：\t";
        cin >> name_;
        cout << "请输入你的性别（0代表男，1代表女）：\t";
        int gender;
        chk_int_input(gender, 0, 1);
        gender_ = Gender(gender);
        n_ride_ = 0;
        balance_ = 0;
        DBConnector().exec("INSERT INTO student VALUES(" + to_string(id_) + ",\"" + name_ + "\",\"" + to_string((int)gender_) + "\",\"" + school_ + "\",0,0);");
        cout << "恭喜你注册成功";
        return true;
    }
}

void Teacher::show_info() {
    Card::show_info();
    cout << "学院：" << school_ << '\n';
}

void Teacher::ride(Bus& bus) {
    chk_bus_status_(bus);
    show_info();
}

void Teacher::recharge() {
    cout << "教师无需充值一卡通，请免费乘车\n";
}

bool Teacher::load_info() {
    auto vec = DBConnector().load("SELECT * FROM " + db_tbl_ + " WHERE id=" + to_string(id_) + ";");
    if (vec.empty()) {
        return false;
    } else {
        name_ = vec[1];
        gender_ = (Card::Gender)stoi(vec[2]);
        school_ = vec[3];
        n_ride_ = stoi(vec[4]);
        return true;
    }
}

void Teacher::sync() {
    DBConnector().exec("UPDATE " + db_tbl_ + " SET name=\"" + name_ + "\",gender=\"" + to_string((int)gender_) + "\",school=\"" + school_ + "\",n_ride=" + to_string(n_ride_) + " WHERE id=" + to_string(id_) + ";");
}

bool Teacher::sign_up_() {
    cout << "请输入你的工资号：\t";
    chk_int_input(id_, 0, 100000);
    if (exist()) {
        cout << "该账号已经被注册，注册失败！\n";
        return false;
    } else {
        cout << "请输入你所在的学院：\t";
        cin >> school_;
        cout << "请输入你的姓名：\t";
        cin >> name_;
        cout << "请输入你的性别（0代表男，1代表女）：\t";
        int gender;
        chk_int_input(gender, 0, 1);
        gender_ = Gender(gender);
        n_ride_ = 0;
        DBConnector().exec("INSERT INTO teacher VALUES(" + to_string(id_) + ", \"" + name_ + "\", \"" + to_string((int)gender_) + "\", \"" + school_ + "\", 0);");
        cout << "恭喜你注册成功";
        return true;
    }
}

void FamMember::show_info() {
    Card::show_info();
    cout << "余额：" << balance_ << '\n';
}

void FamMember::ride(Bus& bus) {
    if (n_ride_ < 19) {
        chk_bus_status_(bus);
    } else if (n_ride_ == 19) {
        cout << "这是本月最后一次免费乘车\n";
        chk_bus_status_(bus);
    } else if (n_ride_ == 20) {
        cout << "本次开始收费\n";
        if (check_balance_()) {
            chk_bus_status_(bus);
        }
    } else {
        if (check_balance_()) {
            chk_bus_status_(bus);
        }
    }
    show_info();
}

bool FamMember::load_info() {
    auto vec = DBConnector().load("SELECT * FROM " + db_tbl_ + " WHERE id=" + to_string(id_) + ";");
    if (vec.empty()) {
        return false;
    } else {
        name_ = vec[1];
        gender_ = (Card::Gender)stoi(vec[2]);
        balance_ = stoi(vec[3]);
        n_ride_ = stoi(vec[4]);
        return true;
    }
}

void FamMember::sync() {
    DBConnector().exec("UPDATE " + db_tbl_ + " SET name=\"" + name_ + "\",gender=\"" + to_string((int)gender_) + "\",balance=" + to_string(balance_) + ",n_ride=" + to_string(n_ride_) + " WHERE id=" + to_string(id_) + ";");
}

bool FamMember::sign_up_() {
    id_ = stoi(DBConnector().load("SELECT MAX(id) FROM fam_member;")[0]) + 1;
    cout << "请输入你的姓名：\t";
    cin >> name_;
    cout << "请输入你的性别（0代表男，1代表女）：\t";
    int gender;
    chk_int_input(gender, 0, 100000);
    gender_ = Gender(gender);
    n_ride_ = 0;
    balance_ = 0;
    DBConnector().exec("INSERT INTO fam_member VALUES(" + to_string(id_) + ", \"" + name_ + "\", \"" + to_string((int)gender_) + "\", 0, 0);");
    cout << "恭喜你注册成功";
    return true;
}

bool LimitedCard::check_balance_() {
    if (balance_ < 2) {
        cout << "余额不足，拒绝乘车，请充值后重试\n";
        return false;
    } else {
        if (balance_ <= 5) {
            cout << "余额过低，及时充值。\n";
        }
        balance_ -= 2;
        return true;
    }
}

void LimitedCard::recharge() {
    cout << "您想要充值多少元？\n";
    int money;
    chk_int_input(money, 0, 100);
    balance_ += money;
    sync();
    cout << "充值成功，您当前的余额为" << balance_ << "\n";
}
