// 回避多线程的安全问题
#include <iostream>
#include <map>
#include <stdio.h>

class Singleton;
static std::map<std::string, Singleton*> myMap = std::map<std::string, Singleton*>();

// 懒汉-延迟加载
class Singleton
{
private:
    Singleton() {
        m_singer = NULL;
        std::cout << "单例正在被构建" << std::endl;
    }
public:
    static Singleton* getInstance() {
        // std::map<std::string, int>::iterator it = myMap.find(DEFAULT_KEY);
        if (myMap.find(DEFAULT_KEY) != myMap.end())
            return myMap.find(DEFAULT_KEY)->second;
        if (m_singer == NULL) {
            m_singer = new Singleton;
            myMap[DEFAULT_KEY] = m_singer;
        }
        return m_singer;
    }
private:
    static Singleton* m_singer;
    static std::string DEFAULT_KEY;
};

Singleton* Singleton::m_singer = NULL;
std::string Singleton::DEFAULT_KEY = "One";


int main(int argc, char* argv[])
{
    Singleton* p1 = Singleton::getInstance();
    Singleton* p2 = Singleton::getInstance();

    std::cout << "&p1 = " << p1 << ", &p2 = " << p2 << std::endl;
    /*
    result:
        单例正在被构建
        &p1 = 0x55e7d4d30eb0, &p2 = 0x55e7d4d30eb0
    */

    return 0;
}