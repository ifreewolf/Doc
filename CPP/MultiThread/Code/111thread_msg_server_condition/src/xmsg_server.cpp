#include "xmsg_server.h"
#include <iostream>

// 给当前线程发消息
void XMsgServer::SendMsg(std::string msg)
{
    std::unique_lock<std::mutex> lock(mux_);    // 确保单线程访问
    msgs_.push_back(msg);
    lock.unlock();
    cv_.notify_one();
}


// 处理消息的线程入口函数
void  XMsgServer::Main()
{
    while (!is_exit())  // 循环处理
    {
        // std::this_thread::sleep_for(std::chrono::milliseconds(10)); // 消息延迟处理, 因为使用condition_variable的wait方法，会自己阻塞等待，所以不需要延时
        std::unique_lock<std::mutex> lock(mux_); // 每次循环都会释放锁
        cv_.wait(lock, [this] {
                std::cout << "wait cv" << std::endl;
                if (is_exit()) return true;
                return !msgs_.empty(); 
            });  // 当发完10个数据后，在这里被阻塞了。因为线程一直阻塞等待锁

        while (!msgs_.empty()) {
            // 消息处理业务逻辑
            std::cout << "recv: " << msgs_.front() << std::endl;
            msgs_.pop_front();
        }

    }
}

void XMsgServer::Stop()
{
    is_exit_ = true;
    cv_.notify_all();   // 通知所有的线程
    Wait();
}