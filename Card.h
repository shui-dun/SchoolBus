#pragma once
#include <string>
#include "Bus.h"
using std::string;

class Card {
protected:
    int id_;
    string name_;
    enum Gender {
        MAN, WOMAN
    } gender_;
    int n_ride_;
    bool chk_bus_status_(Bus& bus);
    string db_tbl_;
    virtual bool sign_up_() = 0;
public:
    bool exist();
    static void login(Card*& card);
    static void sign_up_entry(Card*& card);
    static void logout_entry(Card*& card);
    bool logout();
    virtual void ride(Bus& bus) = 0;
    virtual void show_info();
    virtual bool load_info() = 0;
    virtual void sync() = 0;
    virtual void recharge() = 0;
    static void chk_int_input(int& data, int min, int max);
};

class LimitedCard :public Card {
protected:
    int balance_;
    bool check_balance_();
public:
    void recharge();
};

class Student :public LimitedCard {
private:
    string school_;
    bool sign_up_();
public:
    Student() { db_tbl_ = "student"; }
    void show_info();
    void ride(Bus& bus);
    bool load_info();
    void sync();
};

class Teacher :public Card {
private:
    string school_;
    bool sign_up_();
public:
    Teacher() { db_tbl_ = "teacher"; }
    void show_info();
    void ride(Bus& bus);
    void recharge();
    bool load_info();
    void sync();
};

class FamMember :public LimitedCard {
private:
    bool sign_up_();
public:
    FamMember() { db_tbl_ = "fam_member"; }
    void show_info();
    void ride(Bus& bus);
    bool load_info();
    void sync();
};