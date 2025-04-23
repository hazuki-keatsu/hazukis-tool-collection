# Hazuki的小工具合集

Hazuki的小工具合集，你可以自由地使用所有的代码。

## Timer

这是一个程序运行计时工具。你可以引用这个头文件，然后在任何你想设置的地方创建Timer实例，时间记录不会影响原有代码的运行，会随着生命周自动销毁。

### 它的特性

1. 支持标准和**自定义输出流**  
2. 支持**自定义格式化输出**
3. 支持**自定义输出文件位置**
4. 支持指定输出数据**精度**
5. 支持**记录commit ID**
6. 支持**跨平台**
7. 支持**起始点**和**终止点**的**自定义**设置
8. 支持**commit ID**的**短链**记录  

## Gray2Mono  

这是一个图像二值化的小程序。通过在命令行窗口输入`gray2mono <input path> <output path> <parameters>`来运行。

### 参数

1. `-t=128`: 阈值，0~255
2. `-r=3`: 窗口大小，只能为奇数

## Split for CPP

**Split for CPP** 是对 CPP 标准库中没有 `split()` 函数的补充。

**Split for CPP** 是基于输入的分隔符`std::string delimeter`来对 `std::string str` 来进行分割，可以将分割的结果存入多种 STL 模板中 (`std::set<std::string>, std::vector<std::string>, std::stack<std::string>, std::queue<std::string>, std::pair<std::string, std::string>`)

### 使用

`split()` 函数接收两个参数：

1. `str` 被分割的字符串
2. `delimiter` 分隔符

同时返回一个参数：

1. `Container` 返回你设定的容器
