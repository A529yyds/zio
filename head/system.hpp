#pragma once
#include <glog/logging.h>
#include <filesystem>
#include <libunwind.h>
#include <fstream>
#include <nlohmann/json.hpp>
extern "C"
{
#include <czmq.h>
}

class NonCopyable
{
protected:
    NonCopyable() = default;  // 允许默认构造函数
    ~NonCopyable() = default; // 允许默认析构函数

    NonCopyable(const NonCopyable &) = delete;            // 禁止复制构造函数
    NonCopyable &operator=(const NonCopyable &) = delete; // 禁止赋值运算符

    NonCopyable(NonCopyable &&) = delete;            // 禁止移动构造函数
    NonCopyable &operator=(NonCopyable &&) = delete; // 禁止移动赋值运算符
};

#define WORKER_READY "\001"

class Singleton
{
private:
    inline Singleton()
    {
        std::filesystem::path p("./config.json");
        std::ifstream ifs(p, std::ifstream::binary);
        std::string content((std::istreambuf_iterator<char>(ifs)), {});
        conf = nlohmann::json::parse(content);
    };
    ~Singleton(){};
    Singleton(const Singleton &);
    Singleton &operator=(const Singleton &);

private:
    nlohmann::json conf;

public:
    static Singleton &getInstance()
    {
        static Singleton instance;
        return instance;
    }
    inline const nlohmann::json &GetConf()
    {
        return conf;
    }
};

namespace CUitl
{
    inline std::string Print_trace()
    {
        unw_cursor_t cursor;
        unw_context_t context;
        std::string temp;
        // 初始化context
        unw_getcontext(&context);
        unw_init_local(&cursor, &context);

        // 遍历调用栈
        while (unw_step(&cursor) > 0)
        {
            unw_word_t offset, pc;
            char fname[64];

            unw_get_reg(&cursor, UNW_REG_IP, &pc);
            fname[0] = '\0';
            unw_get_proc_name(&cursor, fname, sizeof(fname), &offset);
            temp.append(fname);
            temp.append("() ");
            temp.append("+0x");
            temp.append(std::to_string(offset));
            temp.append("\n");
        }
        return temp;
    }
}