


<div align="center">
    <img src="./image/多线程结构图.png" />
</div>


# C++11 14 17多线程从原理到线程池实战

---

## 一. 为什么要用多线程

1. 任务分解

> 耗时的操作，任务分解，实时响应；


2. 数据分解

> 充分利用多核CPU处理数据

3. 数据流分解

> 读写分离，解耦合设计

## 二. C++11多线程快速入门

### 1. 多线程示例

```c++
#include <thread>
#include <iostream>

using namespace std;

void ThreadMain()
{
    cout << "Begin ThreadMain" << endl;
    cout << "ThreadMain ID = " << std::this_thread::get_id() << endl;
    for (int i = 0; i < 10; i++)
    {
        cout << "in thread " << i << endl;
        this_thread::sleep_for(chrono::seconds(1)); // 睡眠1秒钟
    }
    
}

int main(int argc, char* argv[])
{
    cout << "main thread ID = " << std::this_thread::get_id() << endl;
    // 线程创建启动
    std::thread th(ThreadMain);
    cout << "begin wait sub thread!" << endl;
    th.join();   // 阻塞等待子线程退出
    cout << "end wait sub thread!" << endl;
    return 0;
}
```

CMakeLists.txt 文件配置：

```cmake
cmake_minimum_required(VERSION 3.1)
project(first_thead)

set(CMAKE_CXX_STANDARD 11)

find_package(Threads REQUIRED)

add_executable(first_thread 101first_thread.cpp)

target_link_libraries(first_thread Threads::Threads)
```

<B>PS:</B> `th.join()` 阻塞等待子线程退出

创建线程对象，同时把处理函数传递给线程对象，调用`join()`方法阻塞等待线程退出。注意：线程对象创建的时候会自动启动线程，不需要调用`start()`方法。

### 2. std::thread 对象生命周期和线程等待和分离

对于线程函数，以及头文件如下：

```c++
#include <thread>
#include <iostream>

using namespace std;

void ThreadMain()
{
    std::cout << "begin ThreadMain ID = " << std::this_thread::get_id()  << std::endl;
    for (int i = 0; i < 10; i++)
    {
        std::cout << "in thread " << i << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    std::cout << "end ThreadMain ID = " << std::this_thread::get_id()  << std::endl;
    
}
```

CMakeLists.txt 文件配置：

```cmake
cmake_minimum_required(VERSION 3.1)
project(thread_detach VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 11)

find_package(Threads REQUIRED)

add_executable(thread_detach 102thread_detach.cpp)

target_link_libraries(thread_detach Threads::Threads)
```

示例1：

```c++
int main(int argc, char *argv[])
{
    // 示例1
    {   // 大括号结束之后，th这个线程对象就被系统自动销毁了，但此时子线程已经被执行了且没有执行完。
        std::thread th(ThreadMain); // 出错，thread对象被销毁，子线程还在运行
    }
    // 出现错误：
    // terminate called without an active exception
    // Aborted (core dumped)

    return 0;
}
```

执行结果如下所示：

```bash
terminate called without an active exception
Aborted (core dumped)
```

分析：

> 程序中，大括号结束之后，th这个线程对象就被系统自动销毁了，但此时子线程已经被执行了且没有执行完。解决方案：使用`join()`函数等待子线程执行完再销毁。

示例2：`join()`方法

```c++
// 示例2
{   // 使用join()函数，会等待子线程执行完，大括号里面的内容才算结束，所以在子线程执行完之前，线程对象th不会被销毁。
    std::thread th(ThreadMain);
    this_thread::sleep_for(std::chrono::seconds(1));
    is_exit = true; // 通知子线程退出
    std::cout << "主线程阻塞，等待子线程退出！" << std::endl;
    th.join();  // 主线程会被阻塞，等待子线程退出
    std::cout << "子线程已经退出！" << std::endl;
}
```

> 这种情况，程序可以正常执行，但依然存在一个问题，主程序被阻塞了，只有子线程执行完，主线程才会继续执行。解决方案：使用`detach()`让子线程和主线程分离，这样，主线程可以继续执行，子线程独立运行。

示例3：`detach()`方法

```c++
// 示例3
{   // 子线程与主线程分离
    thread th(ThreadMain);
    th.detach();    // 子线程与主线程分离，相当于守护线程
}
// 存在的问题：就得保证子线程不能去访问主线程的资源，否则会有错误。
// 解决方案：1.子线程不访问主线程的资源 2.用一个全局变量去通知子线程，主线程要退出了。
```

> 这种情况，大括号执行完之后，因为子线程和子线程分离了，所以不存在因为线程对象被销毁而导致子线程无法运行的情况。但是依然存在一个问题：主线程退出后，子线程不一定退出。如果子线程要访问主线程的资源就会发生错误，即使不访问主线程的资源在程序退出的时候依然会有错误。最好的解决方案就是：用一个全局变量去通知子线程，主线程要退出了。

示例4：`detach()`方法改进

```c++
bool is_exit = false;

void ThreadMain()
{
    std::cout << "begin ThreadMain ID = " << std::this_thread::get_id()  << std::endl;
    for (int i = 0; i < 10; i++)
    {
        if (is_exit) break;
        std::cout << "in thread " << i << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    std::cout << "end ThreadMain ID = " << std::this_thread::get_id()  << std::endl;
    
}

int main(int argc, char *argv[])
{
    // 示例3
    {   // 子线程与主线程分离
        thread th(ThreadMain);
        th.detach();    // 子线程与主线程分离，相当于守护线程
    }

    return 0;
}
```

至此，就可以比较完美的解决线程分离的问题。


完整文件：

```cpp
#include <thread>
#include <iostream>

using namespace std;

bool is_exit = false;

void ThreadMain()
{
    std::cout << "begin ThreadMain ID = " << std::this_thread::get_id()  << std::endl;
    for (int i = 0; i < 10; i++)
    {
        if (is_exit) break;
        std::cout << "in thread " << i << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    std::cout << "end ThreadMain ID = " << std::this_thread::get_id()  << std::endl;
    
}

int main(int argc, char *argv[])
{
    // 示例1
    {   // 大括号结束之后，th这个线程对象就被系统自动销毁了，但此时子线程已经被执行了且没有执行完。
        // std::thread th(ThreadMain); // 出错，thread对象被销毁，子线程还在运行
    }
    // 出现错误：
    // terminate called without an active exception
    // Aborted (core dumped)

    // 示例3
    {   // 子线程与主线程分离
        thread th(ThreadMain);
        th.detach();    // 子线程与主线程分离，相当于守护线程
    }
    // 存在的问题：主线程退出后，子线程不一定退出。就得保证子线程不能去访问主线程的资源，否则会有错误。
    // 解决方案：1.子线程不访问主线程的资源 2.用一个全局变量去通知子线程，主线程要退出了。

    // 示例2
    {   // 使用join()函数，会等待子线程执行完，大括号里面的内容才算结束，所以在子线程执行完之前，线程对象th不会被销毁。
        std::thread th(ThreadMain);
        this_thread::sleep_for(std::chrono::seconds(1));
        is_exit = true; // 通知子线程退出
        std::cout << "主线程阻塞，等待子线程退出！" << std::endl;
        th.join();  // 主线程会被阻塞，等待子线程退出
        std::cout << "子线程已经退出！" << std::endl;
    }
    // 此时存在的问题是主线程会被阻塞。

    // getchar();

    return 0;
}
```


### 3. C++11线程创建的多种方式和参数传递

#### 1). 普通全局函数作为线程入口

<B>如何传递参数</B>

基于模板函数，优点：任何类型都可以传递。

<B>头文件</B>

1. 值传递

```c++
#include <thread>
#include <iostream>
#include <string>

class Para
{
public:
    Para() { std::cout << "Create Para" << std::endl; }
    Para(const Para& p) { std::cout << "Copy Para" << std::endl; this->name = p.name; }  // 拷贝构造函数
    ~Para() { std::cout << "Drop Para " << std::endl; }
    std::string name;
};


void ThreadMain(int p1, float p2, std::string str, Para p4)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(100));    // 延时 100ms，等待大括号结束，float f2局部变量注销, 这个时候因为是拷贝的参数，所以不会有影响
    std::cout << "ThreadMain: " << p1 << " " << p2 << " " << str << " " << p4.name << std::endl;
}

int main(int argc, char* argv[])
{
    std::thread th;
    {
        float f2 = 12.1f;
        Para p;
        p.name = "test Para class";
        // 所有参数据做复制
        th = std::thread(ThreadMain, 101, f2, "test string para", p);   // 在这里参数的传递是复制
    }
    th.join();

    /*
        三次拷贝：
            1. main函数里面，大括号内，把声明的Para对象p给std::thread()构造函数形参时，会产生一次拷贝；
            2. 回调的时候，会将std::thread()函数的p拷贝到ThreadMain()函数的形参p4
            3. 还有一次是什么？？？？
            我这里出现了1次构造，3次拷贝，4次析构
    */
    return 0;
}
```

运行结果如下：

```bash
Create Para
Copy Para
Copy Para
Drop Para 
Drop Para 
Copy Para
ThreadMain: 101 12.1 test string para test Para class
Drop Para 
Drop Para 
```

可以发现，在std::thread()函数里面发生了3次拷贝，1次构造，4次析构，为什么会有3次拷贝呢？？？

<span style="background-color:red"><B>为什么是3次拷贝？</B></div>

2. 传递引用和指针变量

```cpp
#include <thread>
#include <iostream>
#include <string>

class Para
{
public:
    Para() { std::cout << "Create Para" << std::endl; }
    Para(const Para& p) { std::cout << "Copy Para" << std::endl; this->name = p.name; }  // 拷贝构造函数
    ~Para() { std::cout << "Drop Para " << std::endl; }
    std::string name;
};

// 指针传递
void ThreadMainPtr(Para* p)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); 
    std::cout << "ThreadMainPtr: " << p->name << std::endl;
}

// 引用传递
void ThreadMainRef(Para& p)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); 
    std::cout << "ThreadMainPtr: " << p.name << std::endl;
}

int main(int argc, char* argv[])
{
    {
        // 传递引用
        Para p;
        p.name = "test ref";
        std::thread th(ThreadMainRef, std::ref(p)); // 为什么要使用std::ref()，因为这里是一个回调，系统无法确定p的类型，如果是普通函数，则可以直接使用p
        th.join(); // 等待线程结束，主线程阻塞
    }

    getchar();

    {
        Para p;
        p.name = "test ThreadMainPtr name";
        std::thread th(ThreadMainPtr, &p);
        th.join();  // 等待线程结束，主线程阻塞
    }

    return 0;
}
```

<B>参数传递的一些坑</B>

> 传递空间已经销毁
> 多线程共享访问一块空间
> 传递的指针变量的生命周期小于线程

```cpp
{
    // 传递线程指针
    Para p;
    p.name = "test ThreadMainPtr name";
    std::thread th(ThreadMainPtr, &p);  // 错误，线程访问的p空间会提前释放
    th.detach();
}
// Para 已经释放了
// ThreadMainPtr: �        �Vtr name // 乱码，
```

上面的例子，传递的指针变量的生命周期小于线程。

运行结果如下;

```bash
Create Para
Drop Para 
ThreadMainPtr: P�{�Utr name
```

会出现乱码！

使用成员函数作为线程入口，可以保证参数的生命周期与线程生命周期相同。


#### 2). 成员函数作为线程入口

<B>接口调用和参数传递</B>

```cpp
#include <thread>
#include <iostream>
#include <string>

class MyThread
{
public:
    // 入口线程函数
    void Main()
    {
        std::cout << "MyThread Main " << name << ":" << age << std::endl;
    }
    string name;
    int age = 100;
};

int main(int argc, char* argv[])
{
    MyThread myth;
    myth.name = "Test name 001";
    myth.age = 20;
    std::thread th(&MyThread::Main, &myth); // myth是对象地址
    th.join();
    return 0;
}
```

<B>PS：</B>使用成员函数作为线程入口，`std::thread th(&MyThread::Main, &myth)`，第一个参数是类名::成员函数地址，第二个参数是对象指针。

<B>线程基类封装</B>

```cpp
#include <thread>
#include <iostream>
#include <string>

/*
    设计一个基类，完成线程的启动，停止，等待退出功能
    基类中三个实现的虚函数：Start(), Stop(), Wait()
    线程的业务，在基类函数中，用纯虚函数定义接口
    子类继承父类，并实现业务函数：Main()
*/
class XThread
{
public:
    virtual void Start()
    {
        is_exit_ = false;
        th_ = std::thread(&XThread::Main, this);    
    }
    
    // 设计一个stop函数，线程未完成强制退
    virtual void Stop()
    {
        is_exit_ = true;
        Wait();
    }

    // 必须要一个等待线程，等待线程结束，否则会有段错误：terminate called without an active exception
    virtual void Wait()
    {
        if (th_.joinable()) // 可以join说明程序还未退出
            th_.join();
    }

    bool is_exit() { return is_exit_; }
private:
    virtual void Main() = 0;    // 纯虚函数，派生类必须要实现
    std::thread th_;
    bool is_exit_ = false;
};

class TestXThread : public XThread
{
public:
    void Main() override // override关键字，在编译阶段就可以检查是否父类有相应的函数，检测重写
    {
        std::cout << "TestXThread Main" << std::endl;
        while (!is_exit())  // 上面已经完成业务，等待手动/延时退出
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            std::cout << "." << std::flush;
        }
    }
    std::string name;
};

int main(int argc, char* argv[])
{

    TestXThread testTh;
    testTh.name = "TestXThread name";
    testTh.Start();
    // getchar();  // 等待，手动退出线程
    std::this_thread::sleep_for(std::chrono::seconds(1)); // 等待，手动退出线程
    testTh.Stop();  // 线程退出
    testTh.Wait();
    return 0;
}
```

<B>PS：</B> override关键字，在子类中用来标识这个函数是重写函数，在编译阶段编译器就会去检测是否父类中该虚函数。


#### 3). lambda 临时函数作为线程入口函数

> lambda 函数，其基本格式如下
> [捕捉列表](参数) mutable -> 返回值类型 {函数体}

mutable关键字: 如果需要在const成员方法中修改一个成员变量的值，那么需要将这个成员变量修饰为mutable。即用mutable修饰的成员变量不受const成员方法的限制。

```cpp
#include <thread>
#include <iostream>
#include <string>


class TestLambda
{
public:
    void Start()
    {
        std::thread th([this]() {std::cout << "name = " << name << std::endl; });
        th.join();
    }
    std::string name = "test lambda";
};

int main(int argc, char* argv[])
{
    std::thread th([](int i) { std::cout << "test lambda " << i << std::endl; }, 123);
    th.join();

    TestLambda test;
    test.Start();

    return 0;
}
```



## 三. 多线程通信和同步

### 3.1 多线程状态

#### 3.1.1 线程状态说明

- 初始化(Init)：该线程正在被创建
- 就绪(Ready)：该线程在就绪列表中等待CPU调度。
- 运行(Running)：该线程正在运行。
- 阻塞(Blocked)：该线程被阻塞挂起。Blocked状态包括：pend(锁、事件、信号量等阻塞)、suspend(主动 pend)、delay(延时阻塞)、pendtime(因为锁、事件、信号量时间等超时等待)。
- 退出(Exit)：该线程运行结束，等待父线程回收其控制块资源。

<div align="cener">
    <img src="./image/线程状态.png" />
</div>

#### 3.1.2 竞争状态(Race Condition)和临界区(Critical Section)

> 竞争状态（Race Condition）

多线程同时读写共享数据

> 临界区

读写共享数据的代码片段。

避免竞争状态策略，对临界区进行保护，同时只能有一个线程进入临界区。

### 3.2 互斥体和锁 mutex

#### 3.2.1 互斥锁 mutex

- 不用锁的情况演示
- 期望输出一整段内容
- lock 和 try_lock()
- unlock()

先来看一个没有互斥锁的情况：

```cpp
#include <thread>
#include <iostream>
#include <string>
#include <mutex>

static std::mutex  mux;

void TestThread()
{
    std::cout << "================================" << std::endl;
    std::cout << "test 001" << std::endl;
    std::cout << "test 002" << std::endl;
    std::cout << "================================" << std::endl;
    mux.unlock();
}


int main(int argc, char* argv[])
{
    // 开 10 个线程，运行上面函数
    for (int i = 0; i < 10; ++i)
    {
        std::thread th(TestThread);
        th.detach();
    }

    return 0;
}
```

运行结果如下所示：

```bash
================================================================
test 001
test 002
================================

test 001
test 002
================================
================================================================
================================test 001
test 002

================================
test 001
test 002
================================

test 001
test 002
================================
================================
test 001
test 002
================================================================
================================test 001
================================
test 001

test 002
================================

test 001
test 002
================================
test 002
================================
test 001
test 002
================================
================================
```

线程函数中的输出存在不能完整输出的情况，因为此时有10个线程在竞争这些资源。

加上互斥锁的情况：

```cpp
#include <thread>
#include <iostream>
#include <string>
#include <mutex>

static std::mutex  mux;

void TestThread()
{

    // 获取锁资源，如果没有则阻塞等待，lock()保护的代码块就是临界区
    // 尽管申请，尽早释放，临界区越小越好
    mux.lock(); // 一旦有资源，进入就绪状态，等待cpu调度
    std::cout << "================================" << std::endl;
    std::cout << "test 001" << std::endl;
    std::cout << "test 002" << std::endl;
    std::cout << "test 003" << std::endl;
    std::cout << "================================" << std::endl;
    mux.unlock();
}


int main(int argc, char* argv[])
{
    // 开 10 个线程，运行上面函数
    for (int i = 0; i < 10; ++i)
    {
        std::thread th(TestThread);
        th.detach();
    }

    getchar();

    return 0;
}
```

运行结果如下：

```bash
================================
test 001
test 002
test 003
================================
================================
test 001
test 002
test 003
================================
================================
test 001
test 002
test 003
================================
================================
test 001
test 002
test 003
================================
================================
test 001
test 002
test 003
================================
================================
test 001
test 002
test 003
================================
================================
test 001
test 002
test 003
================================
================================
test 001
test 002
test 003
================================
================================
test 001
test 002
test 003
================================
================================
test 001
test 002
test 003
================================
```

lock()函数保护的代码块就是临界区，每个线程访问时都会去尝试拿互斥锁，拿到后才能运行临界区的代码，否则线程进入阻塞状态，继续等待资源。

再看看try_lock()的情况：

```cpp
#include <thread>
#include <iostream>
#include <string>
#include <mutex>

static std::mutex  mux;

void TestThread()
{
    for(;;)
    {
        // 获取锁资源，如果没有则阻塞等待，lock()保护的代码块就是临界区
        // 尽晚申请，尽早释放，临界区越小越好
        // mux.lock(); // 一旦有资源，进入就绪状态，等待cpu调度
        // lock()在没获取到资源的时候，就会退出，没有资源消耗。
        // try_lock() 会有资源消耗，所以try_lock()之后必须得等待一段时间，不然会把资源消耗完。
        // try_lock() 的优势，尝试去锁，如果没有获取到锁，也可以做一些操作，比如：休眠、刷新页面等。
        if (!mux.try_lock())
        {
            std::cout << "." << std::flush;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }
        std::cout << "================================" << std::endl;
        std::cout << "test 001" << std::endl;
        std::cout << "test 002" << std::endl;
        std::cout << "test 003" << std::endl;
        std::cout << "================================" << std::endl;
        mux.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));    // 睡眠一会儿，防止资源被消耗完。
    }
}


int main(int argc, char* argv[])
{
    // 开 10 个线程，运行上面函数
    for (int i = 0; i < 10; ++i)
    {
        std::thread th(TestThread);
        th.detach();
    }

    getchar();

    return 0;
}
```

10个线程都进入for循环里面，10个线程同时竞争互斥锁。使用try_lock()去尝试获取锁，如果没有获取到锁，则进行休眠100ms，相当于间隔100ms尝试获取一次锁，不需要太频繁的去获取锁。同时try_lock()相比lock()会有资源消耗，所以需要休眠100ms，否则容易将资源消耗完。


#### 3.2.2 互斥锁的坑_线程抢占不到资源

上面例子中，理论上：1个线程抢占了资源，其他9个线程都在等待。等这个线程运行完之后释放锁，其他9个线程的其中一个可以获取到锁，进入就绪状态等待CPU调度。

但现实中，有可能出现，1个线程抢占了资源，另外9个线程都在等待。这个线程执行完释放锁后，又再次获取到锁了，又继续运行，从而可能出现只有一个线程在运行，其他9个线程一直在等待。

代码如下：

```cpp
#include <thread>
#include <iostream>
#include <string>
#include <mutex>

static std::mutex  mux;

void ThreadMainMux(int i)
{
    for(;;)
    {
        mux.lock();
        std::cout << i << "[in]" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        mux.unlock();
    }
}

int main(int argc, char* argv[])
{
    for (int i = 0; i < 3; ++i)
    {
        std::thread th(ThreadMainMux, i+1);
        th.detach();
    }

    getchar();

    return 0;
}
```

运行结果，如下所示：

```bash
1[in]
1[in]
1[in]
1[in]
1[in]
1[in]
1[in]
1[in]
1[in]
1[in]
1[in]
1[in]
1[in]
1[in]
1[in]
1[in]
1[in]
1[in]
1[in]
1[in]
1[in]
1[in]
1[in]
1[in]
1[in]
1[in]
```

在这个运行过程中，始终只有一个线程能获取得到互斥锁。线程1在unlock()之后又进入了lock()并获得了资源。

改进方式：

```cpp
#include <thread>
#include <iostream>
#include <string>
#include <mutex>

static std::mutex  mux;

void TestThread()
{
    for(;;)
    {
        // 获取锁资源，如果没有则阻塞等待，lock()保护的代码块就是临界区
        // 尽晚申请，尽早释放，临界区越小越好
        // mux.lock(); // 一旦有资源，进入就绪状态，等待cpu调度
        // lock()在没获取到资源的时候，就会退出，没有资源消耗。
        // try_lock() 会有资源消耗，所以try_lock()之后必须得等待一段时间，不然会把资源消耗完。
        // try_lock() 的优势，尝试去锁，如果没有获取到锁，也可以做一些操作，比如：休眠、刷新页面等。
        if (!mux.try_lock())
        {
            std::cout << "." << std::flush;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }
        std::cout << "================================" << std::endl;
        std::cout << "test 001" << std::endl;
        std::cout << "test 002" << std::endl;
        std::cout << "test 003" << std::endl;
        std::cout << "================================" << std::endl;
        mux.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));    // 睡眠一会儿，防止资源被消耗完。
    }
}

void ThreadMainMux(int i)
{
    for(;;)
    {
        mux.lock();
        std::cout << i << "[in]" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        mux.unlock();
        // unlock() 之后，一定要留一部分时间，给系统处理时间。让当前线程还来不及去抢占互斥锁，给其他线程更多的机会能获取到互斥锁。
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}
```

 unlock() 之后，留一部分时间，给系统处理时间。让当前线程还来不及去抢占互斥锁，给其他线程更多的机会能获取到互斥锁。


 #### 3.2.3 超时锁应用 timed_mutex(避免长时间死锁)

 - 可以记录锁获取情况，多次超时，可以记录日志，获取错误情况。

 隔一段时间去尝试获取一下锁，长时间抢占失败后，记录日志。

 try_lock() + sleep(), 在C++17之后，有一个函数：`try_lock_for(milliseconds(1000))`可以完成这两个操作。

 ```cpp
#include <thread>
#include <iostream>
#include <string>
#include <mutex>

std::timed_mutex tmux;
void ThreadMainTime(int i)
{
    for (;;)
    {
        if (tmux.try_lock_for(std::chrono::milliseconds(5000)))
        {
            std::cout << i << "[try_lock_for timeout]" << std::endl;
            continue;
        }
        std::cout << i << "[in]" << std::endl;
        tmux.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

int main(int argc, char* argv[])
{
    for (int i = 0; i < 3; ++i)
    {
        std::thread th(ThreadMainTime, i+1);
        th.detach();
    }
    getchar();

    return 0;
}
 ```



#### 3.2.4 递归锁(可重入)recursive_mutex和recursive_timed_mutex用于业务组合

- 同一个线程中的同一把锁可以锁多次，避免了一些不必要的死锁；
- 组合业务 用到同一个锁

正常情况下，一个互斥锁只能lock()一次。
调用多次lock()时，会出现阻塞等待锁的情况；递归锁可进行多次lock()，每次lock()，计数器+1，一个lock() 对应一个unlock()。

```cpp
#include <thread>
#include <iostream>
#include <string>
#include <mutex>

std::recursive_mutex rmux;
void Task1()
{
    rmux.lock();
    std::cout << "task1 [in]" << std::endl;
    rmux.unlock();
}

void Task2()
{
    rmux.lock();
    std::cout << "task2 [in]" << std::endl;
    rmux.unlock();
}

void ThreadMainRec(int i)
{
    for (;;)
    {
        // 在这个业务流程中，希望Task1()和Task2()以及中间的业务一起执行。如果是互斥锁，则要求Task1()在获取互斥锁之前必须先被释放。
        // 在释放后，就会存在一个问题：在释放时可能会被其他线程抢占资源，从而导致Task1()、Task2()以及中间的业务不能一块执行。
        rmux.lock();
        // rmux.unlock();  // 如果是互斥锁，在这里要执行unlocck()，否则Task1()里面将进入死锁。
        Task1();    // 在Task1()里面又进行了一次lock()，相当于可重入锁。
        std::cout << i << "[in]" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(2));   // 延迟 2 s
        Task2();
        rmux.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

int main(int argc, char* argv[])
{
    for (int i = 0; i < 3; ++i)
    {
        std::thread th(ThreadMainRec, i+1);
        th.detach();
    }
    getchar();

    return 0;
}
```

> 在这个业务流程中，希望Task1()和Task2()以及中间的业务一起执行。如果是互斥锁，则要求Task1()在获取互斥锁之前必须先被释放。
> 
> 在释放后，就会存在一个问题：在释放时可能会被其他线程抢占资源，从而导致Task1()、Task2()以及中间的业务不能一块执行。

#### 3.2.5 共享锁 shared_mutex

- c++14 共享超时互斥锁 shared_timed_mutex
- c++17 共享互斥 shared_mutex
- 如果只有写时需要互斥，读取时不需要，用普通的锁的话如何做？
- 按照如下代码，读取只有一个线程进入，在很多业务场景中，没有充分利用 cpu 资源。

使用场景：
    一个线程要读资源，一个线程要写入资源；多个线程读资源的时候，可同时读取资源，但是多个线程写入资源就会存在问题。

    只要有一个线程在写，则其他所有的线程不能读也不能写；
    相反，有一个线程在读，则其他所有线程可以读，但都不能写；一定要等所有线程读完才能写。

<div align="center">
    <img src="./image/共享锁.png" />
</div>

示例：

```cpp
#include <thread>
#include <iostream>
#include <string>
#include <mutex>
#include <shared_mutex>

std::shared_timed_mutex stmux;

void ThreadRead(int i)
{
    for (;;)
    {
        stmux.lock_shared();
        std::cout << i << " Read" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        stmux.unlock_shared();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void ThreadWrite(int i)
{
    for (;;)
    {
        stmux.lock_shared();
        // 读取数据
        stmux.unlock_shared();
        stmux.lock();   // 互斥锁 写入
        std::cout << i << " Write" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        stmux.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

    }
}

int main(int argc, char* argv[])
{
    for (int i = 0; i < 3; ++i)
    {
        std::thread th(ThreadWrite, i+1);
        th.detach();
    }
    for (int i = 0; i < 3; ++i)
    {
        std::thread th(ThreadRead, i+1);
        th.detach();
    }
    
    getchar();
    return 0;
}
```

可以发现，shared_time_mutex中既有共享锁(lock_shared()，unlock_shared())也有互斥锁(stmux.lock(), unlock())。在读写共享内存业务时，可以利用共享锁。

### 3.3 利用栈特性自动释放锁 RAII

#### 3.3.1 什么是RAII，手动代码实现

> RAII(Resource Acquisition Is Initialization) C++之父Bjarne Stroustrup提出；使用局部对象来管理资源的技术称之为资源获取即初始化；它的生命周期是由操作系统来管理的，无需人工介入；资源的销毁容易忘记，造成死锁或内存泄漏。

<B>手动实现RAII管理mutex资源</B>


#### 3.3.2 C++11支持的RAII管理互斥锁资源 lock_guard

- c++11 实现严格基于作用域的互斥体所有权包装器；
- adopt_lock C++11类型为adopt_lock_t，假设调用方已拥有互斥的所有权；
- 通过{}控制锁的临界区

```cpp
#include <thread>
#include <iostream>
#include <string>
#include <mutex>
#include <shared_mutex>

// RAII
class XMutex
{
public:
    XMutex(std::mutex &mux) : mux_(mux)
    {
        std::cout << "Lock" << std::endl;
        mux.lock();
    }
    ~XMutex()
    {
        std::cout << "Unlock" << std::endl;
    }
private:
    std::mutex& mux_;// 应用在初始化的时候就需要赋值
};

static std::mutex mux;
void TestMutex(int status)
{
    // 这个锁被传入XMutex对象中，并且在构造函数中被lock(),因此其他任意对象函数都无法再获取该锁。只有XMutex生命周期结束，其他对象和函数才有可能获取到该锁。
    // 局部对象来管理资源。
    XMutex lock(mux);
    if (status == 1)
    {
        std::cout << "=1" << std::endl;
        return;
    }
    else
    {
        std::cout << "!=1" << std::endl;
        return;
    }
}

int main(int argc, char* argv[])
{
    TestMutex(1);
    TestMutex(2);
    
    getchar();
    return 0;
}
```

上述代码中，展示的是C++11中lock_guard的实现，通过局部对象来管理资源。

下面看看lock_guard的使用：

```cpp
#include <thread>
#include <iostream>
#include <string>
#include <mutex>
#include <shared_mutex>

static std::mutex gmutex;
void TestLockGuard(int i)
{
    gmutex.lock();
    {
        // std::lock_guard<std::mutex> lock(gmutex);    // 会报错，因为锁已经在外面被占用了
        // 已经拥有锁，不会lock()
        std::lock_guard<std::mutex> lock(gmutex, std::adopt_lock);
        // 结束释放锁
    }

    {   // 这个锁只作用于当前作用域
        std::lock_guard<std::mutex> lock(gmutex);
        std::cout << "begin thread " << i << std::endl;
    }
    for (;;)
    {
        {// 这个锁只作用于当前作用域
            std::lock_guard<std::mutex> lock(gmutex);
            std::cout << "In " << i << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    // lock_guard可以避免频繁的lock和unlock，同时可以更好的控制临界区
}

int main(int argc, char* argv[])
{
    for (int i = 0; i < 3; i++) {
        std::thread th(TestLockGuard, i+1);
        th.detach();
    }
    getchar();

    return 0;
}
```

lock_guard可以实现使用局部变量控制临界区，局部变量的作用域就是临界区的范围。同时，`std::lock_guard<std::mutex> lock(gmutex, std::adopt_lock);`可以实现如果已经拥有锁，不会lock()，并且在结束后会释放锁。


#### 3.3.3 unique_lock c++11

- unique_lock C++11 实现可移动的互斥体所有权包装器
- 支持临时释放锁 unlock
- 支持 adopt_lock（已经拥有锁，不加锁，出栈区会释放）
- 支持 defer_lock（延后拥有，不加锁，出栈区不释放）
- 支持 try_to_lock 尝试获得互斥的所有权而不阻塞，获取失败退出栈区不会释放，通过 owns_lock() 函数判断
- 支持超时参数，超时不拥有锁。


unique_lock示例：

```cpp
#include <thread>
#include <iostream>
#include <string>
#include <mutex>
#include <shared_mutex>

int main(int argc, char* argv[])
{

    {
        static std::mutex mux;
        {
            std::unique_lock<std::mutex> lock(mux);
            lock.unlock();
            // 临时释放锁
            lock.lock();
        }

        {   // std::adopt_lock
            // 已经拥有锁，不锁定，退出解锁
            mux.lock();
            std::unique_lock<std::mutex> lock(mux, std::adopt_lock);    // std::adopt_lock：已经拥有锁，则不上锁，退出时会释放锁
        }

        {   // std::defer_lock
            // 延后加锁，不拥有，退出不解锁
            std::unique_lock<std::mutex> lock(mux, std::defer_lock);
            lock.lock();
            // 适用场景：
            //      本次处理完之后，要加锁
        }

        {   // std::try_to_lock
            // mux.lock(); // not owns_lock
            // 尝试加锁 不阻塞 失败不拥有锁
            std::unique_lock<std::mutex> lock(mux, std::try_to_lock);
            if(lock.owns_lock())
            {
                std::cout << "owns_lock" << std::endl;
            }
            else
            {
                std::cout << "not owns_lock" << std::endl;
            }
        }
    }
    getchar();

    return 0;
}
```

#### 3.3.4 shared_lock c++14

shared_lock C++14 实现可移动的共享互斥体所有权封装器
```cpp
explicit shared_lock(mutex_type& m) : _Pmtx(_STD addressof(_Mtx)), _Owns(true) {
    _Mtx.lock_shared(); // 最终还是调用了共享锁
}
```

使用示例：

```cpp
int main(int argc, char* argv[])
{
    {
        // 共享锁
        static std::shared_timed_mutex tmux;
        // 读取锁 共享锁
        {
            // 调用共享锁
            std::shared_lock<std::shared_timed_mutex> lock(tmux);
            std::cout << "read data" << std::endl;
            // 退出栈区 释放共享锁
        }
        // 写入锁 互斥锁
        {
            std::unique_lock<std::shared_timed_mutex> lock(tmux);
            std::cout << "write data" << std::endl;
        }
    }
    getchar();
}
```

上面代码中，`std::shared_lock<std::shared_timed_mutex> lock(tmux);`是调用了共享锁，而`std::unique_lock<std::shared_timed_mutex> lock(tmux);`是调用了互斥锁。

为什么要用这个包装器？与`std::shared_timed_mutex`相比，就是不需要手动lock和unlock，这部分内容的目的就是使用包装器来代替原来的互斥锁等，代替手动加锁和释放锁的过程。


#### 3.3.5 scoped_lock c++17

```cpp
scoped_lock C++17 用于多个互斥体的免死锁 RAII 封装 类似 lock

explicit scoped_lock(_Mutexes&... __Mtxes) : _MyMutexes(_Mtxes...) { // construct and lock
    _STD lock(_Mtxes...);
}

lock(mux1, mux2);
mutex mux1, mux2;
std::scoped_lock lock(mux1, mux2);
```





