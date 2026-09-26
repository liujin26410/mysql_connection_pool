#pragma once
#include <ctime>
#include <iostream>
#include <mysql.h>
#include <string>

class mysqlConnection {
public:
  mysqlConnection();  // 数据库初始化
  ~mysqlConnection(); // 释放数据库连接资源
  bool connect(std::string const &ip, unsigned short port,
               std::string const &username, std::string const &password,
               std::string const &dbname); // 连接数据库

private:
  MYSQL *_conn;       // 表示和MySQL的一个连接
  clock_t _alivetime; // 表示进入空闲时间后的存活时间
};