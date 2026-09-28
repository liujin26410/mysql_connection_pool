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

bool mysqlConnection::update(std::string const &sql) {
  if (mysql_query(_conn, sql.c_str())) {
    return false;
  }
  return true;
}

MYSQL_RES *mysqlConnection::query(std::string const &sql) {
  if (mysql_query(_conn, sql.c_str())) {
    std::cout << "mysql_query失败：" << mysql_error(_conn) << std::endl;
    return nullptr;
  }
  MYSQL_RES *res = mysql_store_result(_conn);
  if (res == nullptr) {
    std::cout << "mysql_store_result失败：" << mysql_error(_conn) << std::endl;
  }
  return res;
}

void mysqlConnection::refreshAliveTime() { this->_alivetime = clock(); }

clock_t mysqlConnection::getAliveTime() const {
  return clock() - this->_alivetime;
}
