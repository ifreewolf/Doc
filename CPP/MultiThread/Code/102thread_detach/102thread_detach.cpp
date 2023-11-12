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