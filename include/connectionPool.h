#pragma once
#include "mysqlConnection.h"
#include <atomic>
#include <cctype>
#include <chrono>
#include <condition_variable>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

// 实现连接池模块
class connectionPool {
public:
  // 获取连接池对象实例
  static connectionPool *getConnectionPool();

  // 给外部接口，从连接池中获取一个可用的空闲连接
  std::shared_ptr<mysqlConnection> getConnection();

private:
  // 单例构造函数私有化
  connectionPool();

  // 从配置文件加载配置项
  bool loadConfigFile();

  // 消除字符串首尾空格
  std::string trim(std::string const &str);

  // 生产者程序: 生产新连接
  void produceConnectionTask();

  // 扫描超过最大空闲时间对连接进行收回
  void scannerConnectionTask();

  std::string _ip;        // mysql的ip地址
  unsigned short _port;   // mysql的端口号 3306
  std::string _username;  // mysql登录用户名
  std::string _password;  // 用户密码
  std::string _dbname;    // 连接数据库名
  int _initSize;          // 最大初始连接量
  int _maxSize;           // 最大连接量
  int _maxIdleTime;       // 最大空闲时间
  int _connectionTimeOut; // 连接池获取连接超时时间

  std::queue<mysqlConnection *> _connectionQue; // 存储mysql的连接队列
  std::mutex _queueMutex;                       // 维护连接队列的线程安全互斥锁
  std::atomic_int _connectionCnt; // 记录连接所创立的mysqlConnection的连接总量
  std::condition_variable cv;     // 设置条件变量,用于连接生产和消费线程通讯
};