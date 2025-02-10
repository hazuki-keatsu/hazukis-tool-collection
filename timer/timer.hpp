/**使用方法：
 * 引用该头文件，在需要计时的函数内第一条语句前创建实例即可
 *
 *      #include "timer.hpp"
 *
 *      自动计数器：
 *      AutoTimer timer(label, mode, format, dst, PRECISION);
 *          不需要手动开始和结束，自动记录时间，输出时间间隔
 *
 *      手动计时器：
 *      ManualTimer timer(label, mode, format, dst, PRECISION);
 *      timer.start();
 *      timer.end();
 *          需要手动调用start()和end()函数开始记录，在程序结束的时候输出时间间隔
 *
 *
 *      参数：
 *      label: 标签，默认为"timer"
 *      mode: 输出模式，默认为"std"，可选"log"
 *      format: 输出格式，默认为"[{time}] ({label}) {duration} seconds."，还有{commitID}可选
 *      dst: 输出目标，默认为"none"，即在工作目录下输出，设置输出路径
 *      PRECISION: 保留小数位数，默认为6
 *
 *      参数可以缺省，但必须顺序填写，否则会出错
 *
 * 该对象示例会在构建时自动初始化，析构时自动输出，不会影响原代码的运行时间
 *
 * 作者：Hazuki
 * 2025-02-09
 */

#ifndef TIMER_HPP
#define TIMER_HPP

#include "./modules/terminalColor_.hpp"
#include "./modules/output_.hpp"

// 自动计时器
class AutoTimer
{
public:
    // 构造函数，记录开始时间
    AutoTimer(const std::string &label = "timer",
              const std::string &mode = "std",
              const std::string &format = "[{time}] ({label}) {duration} seconds.",
              const std::string &dst = "none",
              const int &PRECISION = 6)
        : label_(label), mode_(mode), dst_(dst), PRECISION_(PRECISION), format_(format)
    {
        start_ = std::chrono::high_resolution_clock::now();
    }

    // 析构函数，记录结束时间并输出时间间隔
    ~AutoTimer()
    {
        end_ = std::chrono::high_resolution_clock::now();

        // 时间间隔
        auto duration_ = std::chrono::duration_cast<std::chrono::microseconds>(end_ - start_);

        if (mode_ == "std")
        {
            Output_::stdOutput(label_, duration_, PRECISION_, format_);
        }
        else if (mode_ == "log")
        {
            std::string logFile;
            if (dst_ == "none")
            {
#ifdef _WIN32
                logFile = ".\\timer.log";
#else
                logFile = "./timer.log";
#endif

                Output_::logOutput(label_, duration_, PRECISION_, logFile, format_);
            }
            else
            {
                Output_::logOutput(label_, duration_, PRECISION_, dst_, format_);
            }
        }
    }

private:
    std::chrono::high_resolution_clock::time_point start_;
    std::chrono::high_resolution_clock::time_point end_;
    std::string label_;
    std::string mode_;
    std::string dst_;
    int PRECISION_;
    std::string format_;
};

// 手动计时器
class ManualTimer
{
public:
    ManualTimer(const std::string &label = "timer",
                const std::string &mode = "std",
                const std::string &format = "[{time}] ({label}) {duration} seconds.",
                const std::string &dst = "none",
                const int &PRECISION = 6)
        : label_(label), mode_(mode), dst_(dst), PRECISION_(PRECISION), format_(format)
    {
        ;
    }

    void start()
    {
        start_ = std::chrono::high_resolution_clock::now();
    }

    void end()
    {
        end_ = std::chrono::high_resolution_clock::now();
    }

    ~ManualTimer()
    {
        // 时间间隔
        auto duration_ = std::chrono::duration_cast<std::chrono::microseconds>(end_ - start_);

        if (mode_ == "std")
        {
            Output_::stdOutput(label_, duration_, PRECISION_, format_);
        }
        else if (mode_ == "log")
        {
            std::string logFile;
            if (dst_ == "none")
            {
#ifdef _WIN32
                logFile = ".\\timer.log";
#else
                logFile = "./timer.log";
#endif

                Output_::logOutput(label_, duration_, PRECISION_, logFile, format_);
            }
            else
            {
                Output_::logOutput(label_, duration_, PRECISION_, dst_, format_);
            }
        }
    }

private:
    std::chrono::high_resolution_clock::time_point start_;
    std::chrono::high_resolution_clock::time_point end_;
    std::string label_;
    std::string mode_;
    std::string dst_;
    int PRECISION_;
    std::string format_;
};

#endif