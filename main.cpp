#include <iostream>
#include <map>
#include <vector>

#include "base.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

//声明创建对象的函数
typedef Base *(*create)();

//声明回收对象的函数
typedef void (*destroy)(Base *);

//调用系统函数,加载动态库

#ifdef _WIN32

HINSTANCE loadLib(Base **base, const char *path, const char *funName) {
    auto handle = LoadLibrary(path);
    if (!handle) {
        return nullptr;
    }
    auto cr = (create) GetProcAddress(handle, funName);
    if (cr) {
        *base = cr();
    }
    return handle;
}

//调用系统函数,卸载动态库
void freeLib(HINSTANCE handle, Base *obj, const char *funName) {
    auto free = (destroy) GetProcAddress(handle, funName);
    if (free) {
        free(obj);
    }
    FreeLibrary(handle);
}

struct Module {
  HINSTANCE handle;
  Base *base;
};

#else

void *loadLib(Base **base, const char *path, const char *funName) {
    auto handle = dlopen(path, RTLD_LAZY);
    if (!handle) {
        return nullptr;
    }
    auto cr = (create) dlsym(handle, funName);
    if (cr) {
        *base = cr();
    }
    return handle;
}

//调用系统函数,卸载动态库
void freeLib(void *handle, Base *obj, const char *funName) {
    auto free = (destroy) dlsym(handle, funName);
    if (free) {
        free(obj);
    }
    dlclose(handle);
}

struct Module {
  void *handle;
  Base *base;
};

#endif

//记录模块的map
std::map<std::string, Module> globalM;

//卸载一个模块
void unloadModule(const std::string &moduleName) {
    auto module = globalM.find(moduleName);
    if (module == globalM.end()) {
        return;
    }

    freeLib(module->second.handle, module->second.base, std::string(moduleName + "_destroy").c_str());
}

//加载一个模块
bool loadModule(const std::string &moduleName) {
    std::string libPath;
#ifdef _WIN32
    libPath = std::string("./module/lib" + moduleName + ".dll");
#else
    libPath = std::string("./module/lib" + moduleName + ".so");
#endif
    Base *module = nullptr;
    auto handle = loadLib(&module, libPath.c_str(), std::string(moduleName + "_create").c_str());

    if (!module) {
        std::cout << "load lib" << moduleName << " fail" << std::endl;
        return false;
    }

    if (globalM.find(moduleName) != globalM.end()) {
        unloadModule(moduleName);
    }
    globalM[moduleName] = Module{handle, module};
    return true;
}

std::vector<std::string> splitStr(const std::string &src, const std::string &delimiter) {
    std::vector<std::string> vetStr;

    // 入参检查
    // 1.原字符串为空或等于分隔符，返回空 vector
    if (src.empty() || src == delimiter) {
        return vetStr;
    }
    // 2.分隔符为空返回单个元素为原字符串的 vector
    if (delimiter.empty()) {
        vetStr.push_back(src);
        return vetStr;
    }

    std::string::size_type startPos = 0;
    auto index = src.find(delimiter);
    while (index != std::string::npos) {
        auto str = src.substr(startPos, index - startPos);
        if (!str.empty()) {
            vetStr.push_back(str);
        }
        startPos = index + delimiter.length();
        index = src.find(delimiter, startPos);
    }
    // 取最后一个子串
    auto str = src.substr(startPos);
    if (!str.empty()) {
        vetStr.push_back(str);
    }

    return vetStr;
}

int main() {
    loadModule("module1");

    std::cout << "welcome, input `help` show get all command" << std::endl;

    while (true) {
        std::string line;
        getline(std::cin, line);
        auto lines = splitStr(line, " ");
        if (lines.empty()) {
            continue;
        }
        if (lines[0] == "module") {
            if (lines.size() != 3) {
                std::cout << "command error" << std::endl;
                continue;
            }
            auto v = globalM.find(lines[1]);
            if (v == globalM.end()) {
                std::cout << "module " << lines[1] << "not find" << std::endl;
            } else {
                std::cout << v->second.base->readLine(lines[2]) << std::endl;
            }
        } else if (lines[0] == "load") {
            if (lines.size() < 2) {
                std::cout << "command error" << std::endl;
                continue;
            }
            for (auto iter = lines.begin() + 1; iter != lines.end(); ++iter) {
                if (loadModule("module1")) {
                    std::cout << "load " << iter->c_str() << " ok" << std::endl;
                } else {
                    std::cout << "load " << iter->c_str() << " fail" << std::endl;
                }
            }
        } else if (lines[0] == "list") {
            if (globalM.empty()) {
                std::cout << "<empty>" << std::endl;
                continue;
            }
            for (const auto &k : globalM) {
                std::cout << k.first << std::endl;
            }
        } else if (lines[0] == "help") {
            std::string str("`module moduleName argc` do run module\n");
            str += "`list` show all loaded module\n";
            str += "`load moduleName` to load module\n";
            str += "`help` show help info\n";
            str += "`exit` exit program\n";
            std::cout << str;
        } else if (lines[0] == "exit") {
            std::cout << "godbay" << std::endl;
            break;
        } else {
            std::cout << "unknown command" << std::endl;
        }
    }

    return 0;
}
