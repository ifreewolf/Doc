#ifndef XMSGSERVER_H_
#define XMSGSERVER_H_

#include "xthread.h"
#include <string>
#include <list>
#include <mutex>
#include <condition_variable>

class XMsgServer : public XThread
{
public:
    // 给当前线程发消息
    void SendMsg(std::string msg);

    void Stop() override;
private:
    // 处理消息的线程入口函数
    void  Main() override;

    // 消息队列缓冲
    std::list<std::string> msgs_;

    // 互斥访问消息队列
    std::mutex mux_;

    std::condition_variable cv_;

};

#endif