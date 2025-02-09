/** 使用方法：
 * 引用该头文件，在需要计时的函数内第一条语句前创建实例即可
 *
 *      #include "timer.hpp"
 *
 *      Timer timer("Timer");
 *
 * 该对象示例会在构建时自动初始化，析构时自动输出，不会影响原代码的运行时间
 *
 * 作者：Hazuki
 * 2025-02-09
 */

#ifndef TIMER_HPP
#define TIMER_HPP

#include <iostream>
#include <chrono>
#include <iomanip>
#include <fstream>

#ifdef _WIN32
#include <windows.h>
#endif

// 跨平台终端颜色控制
class TerminalColor_
{
public:
#ifdef _WIN32
    static int getInitialTextAttribute()
    {
        HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        if (GetConsoleScreenBufferInfo(hStdout, &csbi))
        {
            return csbi.wAttributes;
        }
        else
            return -1;
    }

    static void setGreen()
    {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN);
    }

    static void reset()
    {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), textAttribute_);
    }

    // 初始化静态成员变量
    static int textAttribute_;
#else
    static void setGreen()
    {
        std::cout << "\033[1;32m" << std::flush;
    }

    static void reset()
    {
        std::cout << "\033[0m" << std::flush;
    }
#endif
};

// 定义静态成员变量
#ifdef _WIN32
int TerminalColor_::textAttribute_ = TerminalColor_::getInitialTextAttribute();
#endif

class Output_
{
public:
    static void stdOutput(const std::string &label_, std::chrono::microseconds duration, const int &PRECISION)
    {
        std::cout << "Time taken by " << label_ << " is ";
        TerminalColor_::setGreen();
        std::cout << std::fixed << std::setprecision(PRECISION) << (double)duration.count() / 1000000;
        TerminalColor_::reset();
        std::cout << " seconds." << std::endl;
    }

    // static void logOutput(const std::string &label_, std::chrono::microseconds duration, const int &PRECISION)
    // {
    //     std::ofstream logFile("log.txt", std::ios::app);
    //     if (logFile.is_open())
    //     {
    //         logFile << "Time taken by " << label_ << " is ";
    //         TerminalColor_::setGreen();
    //         logFile << std::fixed << std::setprecision(PRECISION) << (double)duration.count
    //     }
    // }
};

// 核心计时器
class Timer
{
public:
    // 输出的小数位数
    static const int PRECISION_ = 6;

    Timer(const std::string &label = "timer", const std::string &model = "std", const std::string &dst = "none")
        : label_(label), model_(model), dst_(dst)
    {
        start_ = std::chrono::high_resolution_clock::now();
    }

    ~Timer()
    {
        end_ = std::chrono::high_resolution_clock::now();
        auto duration_ = std::chrono::duration_cast<std::chrono::microseconds>(end_ - start_);

        if (model_ == "std")
        {
            Output_::stdOutput(label_, duration_, PRECISION_);
        }
        else if (model_ == "log")
        {
            if (dst_ == "none")
            {
            }
        }
    }

private:
    std::chrono::high_resolution_clock::time_point start_;
    std::chrono::high_resolution_clock::time_point end_;
    std::string label_;
    std::string model_;
    std::string dst_;
};

#endif