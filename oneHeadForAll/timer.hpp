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
 * One Head For All
 * 2025-02-09
 */

#ifndef TIMER_HPP
#define TIMER_HPP

#include <iostream>
#include <chrono>
#include <array>
#include <memory>
#include <iomanip>
#include <filesystem>
#include <fstream>
#include <iostream>
#ifdef _WIN32
#include <windows.h>
#endif

// 跨平台终端颜色控制
class TerminalColor_
{
public:
#ifdef _WIN32

    // 获取初始文本属性
    static int getInitialTextAttribute()
    {
        // 获取标准输出句柄
        HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

        // 获取控制台屏幕缓冲区信息
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        if (GetConsoleScreenBufferInfo(hStdout, &csbi))
        {
            // 返回文本属性
            return csbi.wAttributes;
        }
        else
        {
            // 输出错误信息并返回-1
            std::cerr << "\nFailed to get original text attribute." << std::endl;
            return -1;
        }
    }

    static int textAttribute_;

    static void setGreen()
    {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN);
    }

    static void reset()
    {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), textAttribute_);
    }
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

// 输出控制
class Output_
{
public:
    // 时间戳获取
    static std::string getTimestampNow()
    {
        // 获取当前时间
        auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

        std::stringstream ss;
        ss << std::put_time(std::localtime(&now), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }

    // 执行命令
    static std::string execCommand(const char *cmd)
    {
        std::array<char, 128> buffer;
        std::string result;
        // 删除器
        auto deleter = [](FILE *fp)
        { if (fp) pclose(fp); };
        std::unique_ptr<FILE, decltype(deleter)> pipe(popen(cmd, "r"), deleter);
        if (!pipe)
        {
            std::cerr << "popen() failed!" << std::endl;
            return ""; // 返回空字符串，但可以考虑抛出异常或返回错误码
        }
        while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
        {
            result += buffer.data();
        }
        return result;
    }

    // 替换关键字
    static std::string replaceKeyWord(std::string &format, const std::string &label,
                                      std::chrono::microseconds &duration, const int &PRECISION,
                                      std::string &timestamp, std::string &commitID)
    {
        std::string result = format;
        size_t pos;
        // 替换 {time} 关键字
        while ((pos = result.find("{time}")) != std::string::npos)
        {
            result.replace(pos, 6, timestamp);
        }
        // 替换 {label} 关键字
        while ((pos = result.find("{label}")) != std::string::npos)
        {
            result.replace(pos, 7, label);
        }
        // 替换 {duration} 关键字
        while ((pos = result.find("{duration}")) != std::string::npos)
        {
            std::ostringstream ss;
            ss << std::fixed << std::setprecision(PRECISION) << (double)duration.count() / 1000000;
            result.replace(pos, 10, ss.str());
        }
        // 替换 {commitID} 关键字
        while ((pos = result.find("{commitID}")) != std::string::npos)
        {
            result.replace(pos, 10, commitID);
        }

        // commitID 短链
        std::string commitID_short = commitID.substr(0, 7);
        while ((pos = result.find("{commitID-s}")) != std::string::npos)
        {
            result.replace(pos, 12, commitID_short);
        }
        return result;
    }

    // 标准化输出
    static void stdOutput(std::string &label, std::chrono::microseconds duration, const int &PRECISION, std::string &format)
    {
        std::string timestamp = Output_::getTimestampNow();
        std::string commitID = Output_::execCommand("git rev-parse HEAD");
        while (!commitID.empty() && commitID.back() == '\n')
        {
            commitID.pop_back();
        }
        std::string key = replaceKeyWord(format, label, duration, PRECISION, timestamp, commitID);
        TerminalColor_::setGreen();
        std::cout << "\n"
                  << key << std::endl;
        TerminalColor_::reset();
    }

    // 日志输出
    static void logOutput(const std::string &label, std::chrono::microseconds &duration,
                          const int &PRECISION, const std::string &dst, std::string &format)
    {
#ifdef _WIN32
        std::string dir = dst.substr(0, dst.find_last_of('\\'));
#else
        std::string dir = dst.substr(0, dst.find_last_of('/'));
#endif

        // 创建日志目录
        std::filesystem::path logDir(dir);
        if (!std::filesystem::exists(logDir))
        {
            if (!std::filesystem::create_directories(logDir))
            {
                std::cerr << "\nFailed to create log directory." << std::endl;
            }
        }

        // 创建并追加日志文件
        std::ofstream logFile(dst, std::ios::app);
        if (!logFile.is_open())
        {
            std::cerr << "\nFailed to open log file." << std::endl;
            return; // 返回或抛出异常
        }

        std::string timestamp = Output_::getTimestampNow();

        // "[{time}] ({label}) {duration} seconds.\n"

        std::string commitID = "NOCOMMITID";
        if (format.find("{commitID}") != std::string::npos)
        {
            commitID = execCommand("git rev-parse HEAD");
            // 去除可能的换行符
            while (!commitID.empty() && commitID.back() == '\n')
            {
                commitID.pop_back();
            }
        }

        std::string key = replaceKeyWord(format, label, duration, PRECISION, timestamp, commitID);
        ;

        logFile << key << std::endl;
        logFile.close();
    };
};

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