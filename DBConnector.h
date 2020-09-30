#pragma once
#include <mysql.h>
#include <cstdio>
#include <string>
#include "Card.h"
#include "Bus.h"
using std::string;
using std::vector;


class DBConnector {
private:
    MYSQL mysql_;
    MYSQL_RES* res_;
public:
    DBConnector();
    ~DBConnector();
    void exec(std::string sql);
    vector<std::string> load(std::string sql);
};