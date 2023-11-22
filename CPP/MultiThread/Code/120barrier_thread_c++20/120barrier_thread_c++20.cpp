#include <barrier>
#include <thread>
#include <iostream>

void TestBar(int i, std::barrier<> *bar)
{
    std::this_thread::sleep_for(std::chrono::seconds(i));
    std::cout << "begin wait " << std::endl;
    bar->wait(bar->arrive());   // 期待数 -1 阻塞等待，期待为0是返回
    std::cout << "end wait " << std::endl;
}

int main(int argc, char *argv[])
{
    int count = 3;
    std::barrier bar(count);  // 初始数量，3个线程都到了才会执行

    for (int i = 0; i < count; i++)
    {
        std::thread th(TestBar, i, &bar);
        th.detach();
    }

    getchar();


    return 0;
}