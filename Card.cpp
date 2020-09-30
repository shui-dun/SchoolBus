#include "Card.h"
#include "DBConnector.h"
#include <iostream>
#include <cstdlib>
using std::cout;
using std::cin;
using std::to_string;

bool Card::chk_bus_status_(Bus& bus) {
    if (bus.status()) {
        cout << "�ܱ�Ǹ��У���Ѿ������ˣ���ȴ���һ��У����\n";
    } else if (bus.is_full()) {
        cout << "�ܱ�Ǹ��У��������\n";
    } else if (bus.contains_passenger(id_)) {
        cout << "�����ϳ��������ظ�ˢ��\n";
    } else {
        n_ride_++;
        sync();
        bus.insert_passenger(id_);
        if (bus.is_full()) {
            bus.set_status(true);
        }
        cout << "��ӭ�˳���\n";
        return true;
    }
    return false;
}

bool Card::logout() {
    string ch;
    cout << "��ȷ��Ҫע���˺���ע�����������ܳ���У����ȷ��ע��������y������������n\n";
    cin >> ch;
    if (ch == "y" || ch == "yes") {
        DBConnector().exec("DELETE FROM " + db_tbl_ + " WHERE id=" + to_string(id_) + ";");
        cout << "ע���ɹ���\n";
        return true;
    } else {
        return false;
    }
}

void Card::show_info() {
    cout << "���ã�" << name_ << '\n';
    cout << "ID��" << id_ << '\n';
    cout << "���³˳�������" << n_ride_ << '\n';
}

void Card::chk_int_input(int& data, int min, int max) {
    std::string s;
    bool success = false;
    while (true) {
        cin >> s;
        try {
            data = stoi(s);
            if (data<min || data>max) {
                cout << "������[" << min << ',' << max << "]֮�������\n";
                continue;
            }
            return;
        } catch (const std::invalid_argument&) {
            cout << "���������֡�\n";
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
        cout << "���˳���¼��ǰ�˺š�\n";
    }
    int type;
    cout << "�����������ݣ�(0�����ʦ��1����ѧ����2�����ʦ����)\t";
    chk_int_input(type, 0, 2);
    if (type == 0) {
        card = new Teacher();
    } else if (type == 1) {
        card = new Student();
    } else {
        card = new FamMember();
    }
    cout << "���������ID\t";
    chk_int_input(card->id_, 0, 100000);
    if (card->load_info()) {
        cout << "��½�ɹ�\n";
        card->show_info();
    } else {
        delete card;
        card = nullptr;
        cout << "��½ʧ��\n";
    }
}

void Card::sign_up_entry(Card*& card) {
    int type;
    cout << "��ӭע��һ��ͨ��\n";
    cout << "�����������ݣ�(0�����ʦ��1����ѧ����2�����ʦ����)\t";
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
    cout << "ѧԺ��" << school_ << '\n';
    cout << "��" << balance_ << '\n';
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
    cout << "���������ѧ�ţ�\t";
    chk_int_input(id_, 0, 100000);
    if (exist()) {
        cout << "���˺��Ѿ���ע�ᣬע��ʧ�ܣ�\n";
        return false;
    } else {
        cout << "�����������ڵ�ѧԺ��\t";
        cin >> school_;
        cout << "���������������\t";
        cin >> name_;
        cout << "����������Ա�0�����У�1����Ů����\t";
        int gender;
        chk_int_input(gender, 0, 1);
        gender_ = Gender(gender);
        n_ride_ = 0;
        balance_ = 0;
        DBConnector().exec("INSERT INTO student VALUES(" + to_string(id_) + ",\"" + name_ + "\",\"" + to_string((int)gender_) + "\",\"" + school_ + "\",0,0);");
        cout << "��ϲ��ע��ɹ�";
        return true;
    }
}

void Teacher::show_info() {
    Card::show_info();
    cout << "ѧԺ��" << school_ << '\n';
}

void Teacher::ride(Bus& bus) {
    chk_bus_status_(bus);
    show_info();
}

void Teacher::recharge() {
    cout << "��ʦ�����ֵһ��ͨ������ѳ˳�\n";
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
    cout << "��������Ĺ��ʺţ�\t";
    chk_int_input(id_, 0, 100000);
    if (exist()) {
        cout << "���˺��Ѿ���ע�ᣬע��ʧ�ܣ�\n";
        return false;
    } else {
        cout << "�����������ڵ�ѧԺ��\t";
        cin >> school_;
        cout << "���������������\t";
        cin >> name_;
        cout << "����������Ա�0�����У�1����Ů����\t";
        int gender;
        chk_int_input(gender, 0, 1);
        gender_ = Gender(gender);
        n_ride_ = 0;
        DBConnector().exec("INSERT INTO teacher VALUES(" + to_string(id_) + ", \"" + name_ + "\", \"" + to_string((int)gender_) + "\", \"" + school_ + "\", 0);");
        cout << "��ϲ��ע��ɹ�";
        return true;
    }
}

void FamMember::show_info() {
    Card::show_info();
    cout << "��" << balance_ << '\n';
}

void FamMember::ride(Bus& bus) {
    if (n_ride_ < 19) {
        chk_bus_status_(bus);
    } else if (n_ride_ == 19) {
        cout << "���Ǳ������һ����ѳ˳�\n";
        chk_bus_status_(bus);
    } else if (n_ride_ == 20) {
        cout << "���ο�ʼ�շ�\n";
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
    cout << "���������������\t";
    cin >> name_;
    cout << "����������Ա�0�����У�1����Ů����\t";
    int gender;
    chk_int_input(gender, 0, 100000);
    gender_ = Gender(gender);
    n_ride_ = 0;
    balance_ = 0;
    DBConnector().exec("INSERT INTO fam_member VALUES(" + to_string(id_) + ", \"" + name_ + "\", \"" + to_string((int)gender_) + "\", 0, 0);");
    cout << "��ϲ��ע��ɹ�";
    return true;
}

bool LimitedCard::check_balance_() {
    if (balance_ < 2) {
        cout << "���㣬�ܾ��˳������ֵ������\n";
        return false;
    } else {
        if (balance_ <= 5) {
            cout << "�����ͣ���ʱ��ֵ��\n";
        }
        balance_ -= 2;
        return true;
    }
}

void LimitedCard::recharge() {
    cout << "����Ҫ��ֵ����Ԫ��\n";
    int money;
    chk_int_input(money, 0, 100);
    balance_ += money;
    sync();
    cout << "��ֵ�ɹ�������ǰ�����Ϊ" << balance_ << "\n";
}
