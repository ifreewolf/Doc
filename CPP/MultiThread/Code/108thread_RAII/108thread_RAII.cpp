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