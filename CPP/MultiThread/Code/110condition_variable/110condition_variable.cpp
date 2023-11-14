#include <thread>
#include <iostream>
#include <mutex>
#include <list>
#include <string>
#include <sstream>
#include <condition_variable>

std::list<std::string> msgs_;
std::mutex mux;
std::condition_variable cv;

void ThreadWrite()
{
    for (int i = 0; ; i++)
    {
        std::stringstream ss;
        ss << "Write msg " << i;
        std::unique_lock<std::mutex> lock(mux);
        msgs_.push_back(ss.str());
        lock.unlock();  // 为什么要进行解锁？因为如果不解锁，notify之后，有线程被通知了也拿不到锁
        // cv.notify_one();   // 通知其中一个
        cv.notify_all();    // 给所有线程发送信号，但依然只有一个线程能进入
        std::this_thread::sleep_for(std::chrono::seconds(1));    // 每隔一秒写入一次数据
    }
}

void ThreadRead(int i)
{
    for (;;)
    {
        std::cout << "read msg" << std::endl;
        std::unique_lock<std::mutex> lock(mux);
        cv.wait(lock);  // 阻塞等待信号 解锁
        // cv.wait(lock, [i]() {
        //     // std::cout << i << " wait" << std::endl;
        //     // return false;
        //     // return true;    // 返回true则不会阻塞，但是也无法进入，因为未拿到锁
        //     return !msgs_.empty();
        // });   // 阻塞等待可以添加条件，只有满足条件才会进入，否则会继续wait()
        // 获取信号后锁定
        while (!msgs_.empty())
        {
            std::cout << i << " read " << msgs_.front() << std::endl;
            msgs_.pop_front();
        }
    }
}


int main(int argc, char* argv[])
{
    std::thread th(ThreadWrite);
    th.detach();

    for (int i = 0; i < 3; ++i)
    {
        std::thread th(ThreadRead, i+1);
        th.detach();
    }

    getchar();

    return 0;
}