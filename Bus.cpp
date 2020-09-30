#include "Bus.h"
#include "DBConnector.h"
#include <iostream>
#include <random>
#include <ctime>
using namespace std;

std::vector<int> Bus::n_rand(int begin, int end, int n) {
    int size = end - begin;
    std::vector<int> vec(size);
    for (int i = 0; i < size; i++) {
        vec[i] = begin + i;
    }
    std::shuffle(vec.begin(), vec.end(), std::default_random_engine((unsigned)time(NULL)));
    return std::vector(vec.begin(), vec.begin() + n);
}

void Bus::load_info(int id) {
    id_ = id;
    vector<std::string> vec = DBConnector().load("SELECT capacity,license_num,model,driver FROM bus WHERE id=" + std::to_string(id_) + ";");
    capacity_ = stoi(vec[0]);
    srand((unsigned)time(nullptr));
    n_vacant_ = rand() % 10;
    license_num_ = vec[1];
    model_ = vec[2];
    driver_ = vec[3];
    status_ = false;
    passengers_.clear();
}

bool Bus::is_full() {
    return n_vacant_ == 0;
}

void Bus::show_info() {
    cout << "车辆基本信息";
    cout << "\n载乘人数为\t" << capacity_;
    cout << "\n空闲座位数目为\t" << n_vacant_;
    cout << "\n车牌号为\t" + license_num_;
    cout << "\n型号为\t\t" + model_;
    cout << "\n驾驶员为\t" + driver_ + '\n';
}

bool Bus::status() {
    return status_;
}

void Bus::set_status(bool status) {
    status_ = status;
}

bool Bus::contains_passenger(int id) {
    return std::find(passengers_.begin(), passengers_.end(), id) != passengers_.end();
}

void Bus::insert_passenger(int id) {
    passengers_.push_back(id);
    n_vacant_--;
}
