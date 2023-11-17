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

    // MyTask task1;
    // task1.name = "test name 001";
    // pool.AddTask(&task1);

    // MyTask task2;
    // task2.name = "test name 002";
    // pool.AddTask(&task1);

    {
        auto task3 = std::make_shared<MyTask>();
        task3->name = "test name 003";
        pool.AddTask(task3);

        auto task4 = std::make_shared<MyTask>();
        task3->name = "test name 004";
        pool.AddTask(task4);

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    std::cout << "run task count = " << pool.task_run_count() << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(1));
    pool.Stop();
    std::cout << "run task count = " << pool.task_run_count() << std::endl;

    // getchar();
    return 0;
}


// 风险点1：
//  task1和task2是将指针传递到AddTask里面，万一这两个对象被提前销毁了怎么办？解决方案：智能指针