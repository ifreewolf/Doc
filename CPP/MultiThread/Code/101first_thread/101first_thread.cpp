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
    thread th(ThreadMain);
    cout << "begin wait sub thread!" << endl;
    th.join();   // 阻塞等待子线程退出
    cout << "end wait sub thread!" << endl;
    return 0;
}