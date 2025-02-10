#include <iostream>
#include <cstdlib>
#include <random>
#include "./timer/timer.hpp"

AutoTimer timer("auto", "std", "[{time}] ({label}) <{commitID-s}> {duration} seconds.", "none", 6);
ManualTimer timer1("manual1", "std", "[{time}] ({label}) {duration} seconds.", "none", 6);
ManualTimer timer2("manual2", "std", "[{time}] ({label}) {duration} seconds.", "none", 6);

double estimate_pi(long long total_points)
{
    std::random_device rd;  // 获取一个随机数种子
    std::mt19937 gen(rd()); // 使用Mersenne Twister算法生成伪随机数
    std::uniform_real_distribution<> dis(0.0, 1.0);

    long long inside_circle = 0;
    for (long long i = 0; i < total_points; ++i)
    {
        double x = dis(gen);
        double y = dis(gen);
        if (x * x + y * y <= 1.0)
        {
            ++inside_circle;
        }
    }

    return 4.0 * inside_circle / total_points;
}

int main()
{
    std::cout << "开始估计PI值..." << std::endl;

    timer1.start();
    for (int i = 0; i < 10000; i++)
    {
        std::cout << i << " ";
        std::cout << random() << " ";
    }
    timer1.end();

    // 设置一个较大的数字以增加计算时间
    long long total_points = 10000000LL;

    timer2.start();
    double pi_estimate = estimate_pi(total_points);
    timer2.end();

    std::cout << "估算的PI值: " << pi_estimate << std::endl;

    return 0;
}