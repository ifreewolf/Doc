#include <thread>
#include <iostream>
#include <string>

using namespace std;

class MyThread
{
public:
    // 入口线程函数
    void Main()
    {
        std::cout << "MyThread Main " << name << ":" << age << std::endl;
    }
    string name;
    int age = 100;
};

/*

*/
class XThread
{
public:
    virtual void Start()
    {
        is_exit_ = false;
        th_ = std::thread(&XThread::Main, this);    
    }
    
    // 设计一个stop函数，线程未完成强制退
    virtual void Stop()
    {
        is_exit_ = true;
        Wait();
    }

    // 必须要一个等待线程，等待线程结束，否则会有段错误：terminate called without an active exception
    virtual void Wait()
    {
        if (th_.joinable()) // 可以join说明程序还未退出
            th_.join();
    }

    bool is_exit() { return is_exit_; }
private:
    virtual void Main() = 0;    // 纯虚函数，派生类必须要实现
    std::thread th_;
    bool is_exit_ = false;
};

class TestXThread : public XThread
{
public:
    void Main() override // override关键字，在编译阶段就可以检查是否父类有相应的函数，检测重写
    {
        std::cout << "TestXThread Main" << std::endl;
        while (!is_exit())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            std::cout << "." << std::flush;
        }
    }
    std::string name;
};

int main(int argc, char* argv[])
{

    TestXThread testTh;
    testTh.name = "TestXThread name";
    testTh.Start();
    getchar();  // 等待，手动退出线程
    std::this_thread::sleep_for(std::chrono::seconds(1));
    testTh.Stop();
    testTh.Wait();
    
    getchar();

    MyThread myth;
    myth.name = "Test name 001";
    myth.age = 20;
    std::thread th(&MyThread::Main, &myth); // myth是对象地址
    th.join();
    return 0;
}