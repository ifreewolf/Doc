#include <thread>
#include <iostream>
#include <future>
#include <string>

std::string TestPack(int index)
{
    std::cout << "begin TestPack " << index << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    return "Test Pack return";
}

int main(int argc, char* argv[])
{
    std::packaged_task<std::string(int)> task(TestPack);    // std::string(int) 这是函数指针
    auto result = task.get_future();

    // task(100);
    std::thread th(std::move(task), 101);

    std::cout << "begin result get" << std::endl;

    // 测试是否超时
    for (int  i = 0; i < 30; ++i) {
        if (result.wait_for(std::chrono::milliseconds(100)) != std::future_status::ready) {
            continue;
        }
    }
    
    if (result.wait_for(std::chrono::milliseconds(100)) == std::future_status::timeout) {
        std::cout << "wait result timeout" << std::endl;
    } else {
        std::cout << "result get " << result.get() << std::endl;
    }

    th.join();

    return 0;
}