#include "xthread_pool.h"
#include <iostream>

//////////////////////////////////////
// 初始化线程池
// @para num 线程数量
void XThreadPool::Init(int num)
{
    std::unique_lock<std::mutex> lock(mux_);
    this->thread_num_ = num;
    std::cout << "Thread pool Init " << num << std::endl;
}


/////////////////////////////////////
// 启动所有线程，必须先调用Init
void XThreadPool::Start()
{
    std::unique_lock<std::mutex> lock(mux_);
    if (thread_num_ <= 0)
    {
        std::cerr << "Please Init XThreadPool" << std::endl;
        return;
    }
    if (!threads_.empty())
    {
        std::cerr << "Thread pool has start!" << std::endl;
        return;
    }
    // 启动线程
    for (int i = 0; i < thread_num_; i++)
    {
        // auto th = new std::thread(&XThreadPool::Run, this);
        auto th = std::make_shared<std::thread>(&XThreadPool::Run, this);   // 智能指针，智能指针是在堆中，不是在栈中，但是智能指针本身是在栈中
        threads_.push_back(th);
    }
}


////////////////////////////////////
// 线程池退出
void XThreadPool::Stop()
{
    is_exit_ = true;
    cv_.notify_all();   // 通知所有线程要结束线程了
    for (auto &th : threads_)
    {
        th->join();
    }

    std::unique_lock<std::mutex> lock(mux_);
    threads_.clear();   // 清理线程
}


// 线程池线程的入口函数
void XThreadPool::Run()
{
    std::cout << "begin XThreadPool Run " << std::this_thread::get_id() << std::endl;

    while (!is_exit())
    {
        auto task = GetTask();
        if (!task) continue;
        ++task_run_count_;
        try
        {
            auto re = task->Run();
            task->SetValue(re);
        }
        catch (...)
        {

        }
        --task_run_count_;

    }

    std::cout << "end XThreadPool Run " << std::this_thread::get_id() << std::endl;
}


void XThreadPool::AddTask(std::shared_ptr<XTask> task)
{
    std::unique_lock<std::mutex> lock(mux_);
    tasks_.push_back(task);
    lock.unlock();
    cv_.notify_one();
    task->is_exit = [this] { return is_exit(); };   // 函数指针
}


// XTask* XThreadPool::GetTask()
std::shared_ptr<XTask> XThreadPool::GetTask()
{
    std::unique_lock<std::mutex> lock(mux_);
    if (tasks_.empty())
    {
        cv_.wait(lock);
    }
    if (is_exit())
        return nullptr;
    if (tasks_.empty())
        return nullptr;
    auto task = tasks_.front();
    tasks_.pop_front();
    return task;
}


XTask::XTask()
{
    std::cout << "XTask con" << std::endl;
}


XTask::~XTask()
{
    std::cout << "XTask decon" << std::endl;
}


XTask::XTask(const XTask& task)
{
    std::cout << "Copy" << std::endl;
}


