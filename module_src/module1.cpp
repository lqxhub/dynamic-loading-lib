//简单的模块 例子
//转大写

#include <algorithm>
#include <string>
#include "../base.h"

class Module1 : public Base {
  std::string readLine(const std::string &str) override {
      std::string str2(str);
      std::transform(str.begin(), str.end(), str2.begin(), ::toupper);
      return str2;
  }
};

//必须实现 moduleName_create 函数,来初始化对象
extern "C" Base *module1_create() {
    return new Module1;
}

//必须实现 moduleName_destroy 函数,来回收对象
extern "C" void module1_destroy(Base *obj) {
    delete obj;
}