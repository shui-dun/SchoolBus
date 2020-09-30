#pragma once
#include <vector>
#include <string>
using std::string;

class Bus {
private:
    int id_;
    int capacity_;
    int n_vacant_;
    string license_num_;
    string model_;
    string driver_;
    bool status_;
    std::vector<int> passengers_;
public:
    void load_info(int index);
    bool is_full();
    void show_info();
    bool status();
    void set_status(bool status);
    bool contains_passenger(int id);
    void insert_passenger(int id);
    static std::vector<int> n_rand(int begin, int end, int n);
};