#include "connectionPool.h"

connectionPool *connectionPool::getConnectionPool() {
  static connectionPool pool;
  return &pool;
}

std::shared_ptr<mysqlConnection> connectionPool::getConnection() {
  std::unique_lock<std::mutex> lock(_queueMutex);
  while (_connectionQue.empty()) {
    if (std::cv_status::timeout ==
        cv.wait_for(lock, std::chrono::milliseconds(_connectionTimeOut))) {
      if (this->_connectionQue.empty()) {
        std::cout << "获取空闲连接超时，.....获取连接失败" << std::endl;
        return nullptr;
      }
    }
  }
  std::shared_ptr<mysqlConnection> sp(
      this->_connectionQue.front(), [&](mysqlConnection *pcon) {
        std::unique_lock<std::mutex> lock(this->_queueMutex);
        pcon->refreshAliveTime();
        _connectionQue.push(pcon);
      });
  this->_connectionQue.pop();
  cv.notify_all();
  return sp;
}

connectionPool::connectionPool() {
  if (!loadConfigFile()) {
    std::cout << "导入配置失败" << std::endl;
  }
  for (int i = 0; i < this->_initSize; i++) {
    mysqlConnection *p = new mysqlConnection();
    p->connect(_ip, _port, _username, _password, _dbname);
    p->refreshAliveTime();
    this->_connectionQue.push(p);
    this->_connectionCnt++;
  }
  std::thread produce(std::bind(&connectionPool::produceConnectionTask, this));
  produce.detach();
  std::thread scanner(std::bind(&connectionPool::scannerConnectionTask, this));
  scanner.detach();
}

bool connectionPool::loadConfigFile() {
  std::ifstream file_Read("conf/mysql.ini");
  if (!file_Read.is_open()) {
    std::cout << "不能打开文件：" << "conf/mysql.ini" << std::endl;
    return false;
  }
  std::string line;
  while (std::getline(file_Read, line)) {
    std::string str = trim(line);
    if (str.empty() || str[0] == ';') {
      continue;
    }
    size_t eqPos = str.find('=');
    if (eqPos == std::string::npos) {
      continue;
    }
    std::string key = trim(str.substr(0, eqPos));
    std::string value = trim(str.substr(eqPos + 1));
    if (key == "ip") {
      this->_ip = value;
    } else if (key == "port") {
      this->_port = static_cast<unsigned short>(atoi(value.c_str()));
    } else if (key == "username") {
      this->_username = value;
    } else if (key == "password") {
      this->_password = value;
    } else if (key == "dbname") {
      this->_dbname = value;
    } else if (key == "initSize") {
      this->_initSize = atoi(value.c_str());
    } else if (key == "maxSize") {
      this->_maxSize = atoi(value.c_str());
    } else if (key == "maxIdleTime") {
      this->_maxIdleTime = atoi(value.c_str());
    } else if (key == "connectionTimeOut") {
      this->_connectionTimeOut = atoi(value.c_str());
    }
  }
  file_Read.close();
  return true;
}

std::string connectionPool::trim(std::string const &str) {
  auto start = str.begin();
  while (start != str.end() &&
         std::isspace(static_cast<unsigned char>(*start))) {
    ++start;
  }
  if (start == str.end())
    return "";
  auto end = str.end();
  do {
    --end;
  } while (end != start && std::isspace(static_cast<unsigned char>(*end)));
  return std::string(start, end + 1);
}

void connectionPool::produceConnectionTask() {
  for (;;) {
    std::unique_lock<std::mutex> lock(_queueMutex);
    while (!this->_connectionQue.empty()) {
      cv.wait(lock);
    }
    if (this->_connectionCnt < this->_maxSize) {
      mysqlConnection *p = new mysqlConnection();
      p->connect(_ip, _port, _username, _password, _dbname);
      p->refreshAliveTime();
      this->_connectionQue.push(p);
      this->_connectionCnt++;
    }
    cv.notify_all();
  }
}

void connectionPool::scannerConnectionTask() {
  for (;;) {
    std::this_thread::sleep_for(std::chrono::seconds(this->_maxIdleTime));
    std::unique_lock<std::mutex> lock(_queueMutex);
    while (this->_connectionCnt > this->_initSize) {
      mysqlConnection *p = this->_connectionQue.front();
      if (p->getAliveTime() > (this->_maxIdleTime * 1000)) {
        this->_connectionQue.pop();
        this->_connectionCnt--;
        delete p;
      } else {
        break;
      }
    }
  }
}
