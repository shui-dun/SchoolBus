#include <iostream>
#include <string>
#include <mysql.h>
#include <Windows.h>
#include "Card.h"
#include "Bus.h"
#include "DBConnector.h"
using namespace std;

DWORD WINAPI bus_thread(LPVOID lpParamter) {
    Bus& bus = *(Bus*)lpParamter;
    int num = stoi(DBConnector().load("SELECT COUNT(*) FROM bus;")[0]);
    while (true) {
        auto vec = Bus::n_rand(1, num + 1, 2);
        bus.load_info(vec[0]);
        Sleep(1000 * 6);
        bus.set_status(true);
        Sleep(1000 * 6);
        bus.load_info(vec[1]);
        Sleep(1000 * 6);
        bus.set_status(true);
        Sleep(1000 * 6);
    }
    return 0L;
}

int main() {
    Bus bus;
    HANDLE hThread = CreateThread(nullptr, 0, bus_thread, (void*)&bus, 0, nullptr);
    Card* card = nullptr;
    int choice;
    while (true) {
        cout << "\n欢迎进入一卡通乘车管理系统\n";
        cout << "请输入要做的操作\n";
        if (card) {
            cout << "0：切换账号\n";
            cout << "1：申请开卡\n";
            cout << "2：充值\n";
            cout << "3：注销\n";
            cout << "4：乘车\n";
            cout << "5：退出系统\n";
            cout << "6：查看校车信息\n";
            cout << "7：查看个人信息\n\n";
            Card::chk_int_input(choice, 0, 7);
            switch (choice) {
            case 0:Card::login(card); break;
            case 1:Card::sign_up_entry(card); break;
            case 2:card->recharge(); break;
            case 3:Card::logout_entry(card); break;
            case 4:card->ride(bus); break;
            case 5:if (card) { delete card; } return 0;
            case 6:bus.show_info(); break;
            case 7:card->show_info(); break;
            }
        } else {
            cout << "0：登录系统\n";
            cout << "1：申请开卡\n";
            cout << "2：退出系统\n";
            cout << "3：查看校车信息\n\n";
            Card::chk_int_input(choice, 0, 3);
            switch (choice) {
            case 0:Card::login(card); break;
            case 1:Card::sign_up_entry(card); break;
            case 2:if (card) { delete card; } return 0;
            case 3:bus.show_info(); break;
            }
        }
    }
    return 0;
}


