#include "mysqlConnection.h"

mysqlConnection::mysqlConnection() {
  // 初始化数据库连接
  _conn = mysql_init(nullptr);
}

mysqlConnection::~mysqlConnection() {
  if (_conn != nullptr) {
    mysql_close(_conn);
  }
}

bool mysqlConnection::connect(std::string const &ip, unsigned short port,
                              std::string const &username,
                              std::string const &password,
                              std::string const &dbname) {
  MYSQL *p =
      mysql_real_connect(_conn, ip.c_str(), username.c_str(), password.c_str(),
                         dbname.c_str(), port, nullptr, 0);
  return p != nullptr;
}
