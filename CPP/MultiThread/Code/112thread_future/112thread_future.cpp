#include <thread>
#include <iostream>
#include <future>
#include <string>

void TestFuture(std::promise<std::string> p)
{
    std::cout << "begin TestFuture" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(3));
    std::cout << "begin set value" << std::endl;
    p.set_value("TestFuture value");
    std::this_thread::sleep_for(std::chrono::seconds(3));
    std::cout << "end TestFuture" << std::endl;
}

int main(int argc, char* argv[])
{
    // 异步传输变量存储
    std::promise<std::string> p;
    // 用来获取线程异步
    auto future = p.get_future();

    auto th = std::thread(TestFuture, std::move(p));   // std::move()之后，p会被移动到TestFuture()函数中，同时main中的p会被销毁。

    std::cout << "begin future.get()" << std::endl;
    std::cout << "future get() = " << future.get() << std::endl;
    std::cout << "end future.get()" << std::endl;
    th.join();

    getchar();
    return 0;
}