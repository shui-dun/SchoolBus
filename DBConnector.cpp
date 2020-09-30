#include "DBConnector.h"
#include "Bus.h"
#include "Card.h"
#include <string>
#include <cstdlib>
using std::to_string;

DBConnector::DBConnector() :res_(nullptr) {
    mysql_init(&mysql_);
    mysql_options(&mysql_, MYSQL_SET_CHARSET_NAME, "gbk");
    if (!mysql_real_connect(&mysql_, "localhost", "root", "gqgsmmmhn0601", "schoolbus", 3306, NULL, 0)) {
        fprintf(stderr, "Failed to connect to database: Error: %s\n", mysql_error(&mysql_));
    }
}

void DBConnector::exec(std::string sql) {
    mysql_query(&mysql_, sql.c_str());
}

vector<std::string> DBConnector::load(std::string sql) {
    vector<std::string> vec;
    mysql_query(&mysql_, sql.c_str());
    res_ = mysql_store_result(&mysql_);
    MYSQL_ROW row_ = mysql_fetch_row(res_);
    if (row_) {
        int len = res_->field_count;
        for (int i = 0; i < len; i++) {
            vec.push_back(row_[i]);
        }
    }
    return vec;
}

DBConnector::~DBConnector() {
    if (res_) {
        mysql_free_result(res_);
    }
    mysql_close(&mysql_);
}
