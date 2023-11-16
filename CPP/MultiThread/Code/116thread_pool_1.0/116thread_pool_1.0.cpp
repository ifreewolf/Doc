#include "xthread_pool.h"
#include <string>
#include <iostream>

class MyTask : public XTask
{
public:
    int Run()
    {
        std::cout << "========================================" << std::endl;
        std::cout << std::this_thread::get_id() << " MyTask " << name << std::endl;
        std::cout << "========================================" << std::endl;
        for (int i = 0; i < 10; i++)
        {
            if (is_exit()) break;
            std::cout << "." << std::flush;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        return 0;
    }
    std::string name;
};

int main(int argc, char* argv[])
{
    XThreadPool pool;
    pool.Init(16);
    pool.Start();

    MyTask task1;
    task1.name = "test name 001";
    pool.AddTask(&task1);

    std::this_thread::sleep_for(std::chrono::seconds(1));
    pool.Stop();

    // getchar();
    return 0;
}