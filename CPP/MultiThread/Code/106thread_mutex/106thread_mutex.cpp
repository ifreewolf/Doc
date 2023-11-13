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

    for (int i = 0; i < 3; ++i)
    {
        std::thread th(ThreadMainTime, i+1);
        th.detach();
    }
    getchar();

    for (int i = 0; i < 3; ++i)
    {
        std::thread th(ThreadMainMux, i+1);
        th.detach();
    }

    getchar();

    // 开 10 个线程，运行上面函数
    for (int i = 0; i < 10; ++i)
    {
        std::thread th(TestThread);
        th.detach();
    }

    getchar();

    return 0;
}