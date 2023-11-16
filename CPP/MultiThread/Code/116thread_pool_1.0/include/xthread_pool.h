#ifndef XTHREADPOOL_H_
#define XTHREADPOOL_H_

#include <thread>
#include <mutex>
#include <vector>
#include <list>
#include <future>
#include <functional>

class XTask
{
public:
    virtual int Run() = 0;
    // 添加一个函数指针，方便XTask访问XThreadPool中的is_exit()方法
    std::function<bool()> is_exit = nullptr;

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
    void AddTask(XTask* task);

    XTask* GetTask();

    // 线程池是否退出
    bool is_exit() { return is_exit_; }

private:
    // 线程池线程的入口函数
    void Run();
    int thread_num_ = 0; // 线程数量
    std::mutex mux_;
    std::vector<std::thread*> threads_;
    std::list<XTask*> tasks_;
    std::condition_variable cv_;  // 条件变量
    bool is_exit_ = false;  // 线程池退出
};

#endif