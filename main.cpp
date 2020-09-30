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
        cout << "\n��ӭ����һ��ͨ�˳�����ϵͳ\n";
        cout << "������Ҫ���Ĳ���\n";
        if (card) {
            cout << "0���л��˺�\n";
            cout << "1�����뿪��\n";
            cout << "2����ֵ\n";
            cout << "3��ע��\n";
            cout << "4���˳�\n";
            cout << "5���˳�ϵͳ\n";
            cout << "6���鿴У����Ϣ\n";
            cout << "7���鿴������Ϣ\n\n";
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
            cout << "0����¼ϵͳ\n";
            cout << "1�����뿪��\n";
            cout << "2���˳�ϵͳ\n";
            cout << "3���鿴У����Ϣ\n\n";
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


