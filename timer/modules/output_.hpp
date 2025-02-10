#ifndef OUTPUT_HPP
#define OUTPUT_HPP

#include <iostream>
#include <chrono>
#include <array>
#include <memory>
#include <iomanip>
#include <filesystem>
#include <fstream>
#include "./terminalColor_.hpp"

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
#endif