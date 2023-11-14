#include <thread>
#include <iostream>
#include <string>
#include <mutex>
#include <shared_mutex>

// lock_guard的实现原理
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


static std::mutex mux1;
static std::mutex mux2;
void TestScope1()
{
    // 模拟死锁 停 100ms 等另一个线程锁mux2
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::cout << std::this_thread::get_id() << " begin mux1 locck" << std::endl;
    // mux1.lock();
    std::cout << std::this_thread::get_id() << " begin mux2 locck" << std::endl;
    // mux2.lock();
    // C++11
    std::lock(mux1, mux2);
    // C++17
    // std::scoped_lock lock(mux1, mux2);  // 解决死锁
    std::cout << std::this_thread::get_id() << " TestScope1" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    mux2.unlock();
    mux1.unlock();
}

void TestScope2()
{
    std::cout << std::this_thread::get_id() << " begin mux2 locck" << std::endl;
    mux2.lock();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::cout << std::this_thread::get_id() << " begin mux1 locck" << std::endl;
    mux1.lock();
    std::cout << std::this_thread::get_id() << " TestScope2" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    mux1.unlock();
    mux2.unlock();
}

int main(int argc, char* argv[])
{
    {
        // 演示死锁情况
        {
            std::thread th(TestScope1);
            th.detach();
        }
        {
            std::thread th(TestScope2);
            th.detach();
        }
    }

    getchar();

    {   // 封装器
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
            lock.lock();    // 主动lock
            // 适用场景：
            //      本次处理完之后，要加锁。
        }

        {   // std::try_to_lock
            // mux.lock(); // not owns_lock
            // 尝试加锁 不阻塞 失败不拥有锁
            std::unique_lock<std::mutex> lock(mux, std::try_to_lock);
            if(lock.owns_lock())
            {
                std::cout << "4owns_lock" << std::endl;
            }
            else
            {
                std::cout << "4not owns_lock" << std::endl;
            }
        }
    }
    getchar();

    for (int i = 0; i < 3; i++) {
        std::thread th(TestLockGuard, i+1);
        th.detach();
    }
    getchar();

    TestMutex(1);
    TestMutex(2);
    
    getchar();
    return 0;
}