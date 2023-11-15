#include <thread>
#include <iostream>
#include <future>
#include <string>

std::string TestAsync(int index)
{
    std::cout << "begin in TestAsync " << std::this_thread::get_id() << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    return "TestAsync string return";
}

int main(int argc, char* argv[])
{

    // 创建异步线程
    // 不创建线程启动异步任务
    std::cout << "main thread id " << std::this_thread::get_id() << std::endl;
    auto future = async(std::launch::deferred, TestAsync, 100);
    std::cout << "begin future get" << std::endl;
    std::cout << "future.get() = " << future.get() << std::endl;
    std::cout << "end future get" << std::endl;

    getchar();

    return 0;
}