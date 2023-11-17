#ifndef XTHREADPOOL_H_
#define XTHREADPOOL_H_

#include <thread>
#include <mutex>
#include <vector>
#include <list>
#include <future>
#include <functional>
#include <atomic>

class XTask
{
public:
    XTask();
    ~XTask();
    XTask(const XTask& task);
    virtual int Run() = 0;
    // 添加一个函数指针，方便XTask访问XThreadPool中的is_exit()方法
    std::function<bool()> is_exit = nullptr;

    
    auto GetReturn()
    {
        // 阻塞等待 set_value
        return p_.get_future().get();
    }

    void SetValue(int v)
    {
        p_.set_value(v);
    }

private:
    std::promise<int> p_;   // 用来接收返回值
};

class XThreadPool
{
public:
    //////////////////////////////////////
    // 初始化线程池
    // @para num 线程数量
    void Init(int num);

    /////////////////////////////////////
    // 启动所有线程，必须先调用Init
    void Start();

    ////////////////////////////////////
    // 线程池退出
    void Stop();

    ////////////////////////////////////
    // 插入任务列表
    // void AddTask(XTask* task);
    void AddTask(std::shared_ptr<XTask> task);

    // XTask* GetTask();
    std::shared_ptr<XTask> GetTask();

    // 线程池是否退出
    bool is_exit() { return is_exit_; }

    int task_run_count() { return task_run_count_; }

private:
    // 线程池线程的入口函数
    void Run();
    int thread_num_ = 0; // 线程数量
    std::mutex mux_;
    // std::vector<std::thread*> threads_;
    std::vector<std::shared_ptr<std::thread>> threads_;    // 使用智能指针优化
    // std::list<XTask*> tasks_;
    std::list<std::shared_ptr<XTask>> tasks_;
    std::condition_variable cv_;  // 条件变量
    bool is_exit_ = false;  // 线程池退出

    std::atomic<int> task_run_count_ = { 0 };   // 正在运行的任务数量，线程安全 (用途：提供线程实时信息)
};

#endif