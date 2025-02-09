#ifndef TERMINALCOLOR_HPP
#define TERMINALCOLOR_HPP

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

#endif