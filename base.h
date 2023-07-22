//
// Created by QX on 2023-7-13.
//

#ifndef DLOAD_BASE_H
#define DLOAD_BASE_H

/**
 * 必须实现 moduleName_create 函数,来初始化对象
 * extern "C" Base *module1_create() {
 *     return new Module;
 * }
 *
 * //必须实现 moduleName_destroy 函数,来回收对象
 * extern "C" void module1_destroy(Base *obj) {
 *     delete obj;
 * }
 */


class Base {
 public:
  virtual std::string readLine(const std::string &) = 0;

  virtual ~Base() = default;
};

#endif //DLOAD_BASE_H
