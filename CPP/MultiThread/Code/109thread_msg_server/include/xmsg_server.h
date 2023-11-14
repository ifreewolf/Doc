#ifndef XMSGSERVER_H_
#define XMSGSERVER_H_

#include "xthread.h"
#include <string>
#include <list>
#include <mutex>

class XMsgServer : public XThread
{
public:
    // 给当前线程发消息
    void SendMsg(std::string msg);
private:
    // 处理消息的线程入口函数
    void  Main() override;

    // 消息队列缓冲
    std::list<std::string> msgs_;

    // 互斥访问消息队列
    std::mutex mux_;

};

#endif