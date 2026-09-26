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

  bool update(std::string const &sql); // 更新操作 insert、delete、update

  MYSQL_RES *query(std::string const &sql); // 查询操作

  void refreshAliveTime(); // 记录连接的起始空闲时间点

  clock_t getAliveTime() const; // 返回存活时间
private:
  MYSQL *_conn;       // 表示和MySQL的一个连接
  clock_t _alivetime; // 表示进入空闲时间后的存活时间
};