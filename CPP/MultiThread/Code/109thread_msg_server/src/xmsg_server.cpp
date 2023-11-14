#include "xmsg_server.h"
#include <iostream>

// 给当前线程发消息
void XMsgServer::SendMsg(std::string msg)
{
    std::unique_lock<std::mutex> lock(mux_);    // 确保单线程访问
    msgs_.push_back(msg);
}


// 处理消息的线程入口函数
void  XMsgServer::Main()
{
    while (!is_exit())  // 循环处理
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10)); // 消息延迟处理
        std::unique_lock<std::mutex> lock(mux_); // 每次循环都会释放锁
        if (msgs_.empty()) {
            continue;
        }

        while (!msgs_.empty()) {
            // 消息处理业务逻辑
            std::cout << "recv: " << msgs_.front() << std::endl;
            msgs_.pop_front();
        }

    }
}
