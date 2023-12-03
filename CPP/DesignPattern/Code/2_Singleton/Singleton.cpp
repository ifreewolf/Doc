#include <iostream>
#include <thread>
#include <mutex>

std::mutex mux;

class Singleton
{
private:
    Singleton() {
        m_singer = NULL;
        std::cout << "构造一个Singleton对象" << std::endl;
    }
public:
    // 懒汉式交互，在调用的时候才开始创建对象，延迟加载
    // 缺点：线程不安全，多个线程时，容易被创建多个线程
    static Singleton* getInstance() {
        if (m_singer == NULL) {
            m_singer = new Singleton;
        }
        return m_singer;
    }
    // 懒汉式：double check，临界区保护
    // static Singleton* getInstance() {
    //     if (m_singer == NULL) {
    //         mux.lock();
    //         if (m_singer == NULL) {
    //             m_singer = new Singleton;
    //         }
    //         mux.unlock();
    //     }
    //     return m_singer;
    // }
    /*
    result:
        构造一个Singleton对象
        ptr of single: 0x7ff6c0000f70, i = 0
        ptr of single: 0x7ff6c0000f70, i = 1
        ptr of single: 0x7ff6c0000f70, i = 2
        ptr of single: 0x7ff6c0000f70, i = 3
        ptr of single: 0x7ff6c0000f70, i = 4
        ptr of single: 0x7ff6c0000f70, i = 5
        ptr of single: 0x7ff6c0000f70, i = 6
        ptr of single: 0x7ff6c0000f70, i = 7
        ptr of single: 0x7ff6c0000f70, i = 8
        ptr of single: 0x7ff6c0000f70, i = 9
    整个过程只构建了一个对象。
    */
private:
    static Singleton *m_singer;
};

// Singleton* Singleton::m_singer = NULL; // new Singleton; 在c++中，构造函数不是线程安全的
Singleton* Singleton::m_singer = new Singleton; // 饿汉式

void constructorSingleton(int i) {
    std::cout << "ptr of single: " << Singleton::getInstance() << ", i = " << i << std::endl;
}

int main(int argc, char* argv[])
{
    // Singleton *p = new Singleton;   // error, inaccessible
    // Singleton *p1 = Singleton::getInstance();
    // Singleton *p2 = Singleton::getInstance();

    // std::cout << "&p1 = " << p1 << std::endl;
    // std::cout << "&p2 = " << p2 << std::endl;
    // printf("p1 = %x, p2 = %x\n", p1, p2);

    /*
    result:
        构造一个Singleton对象
        &p1 = 0x556cd1f41eb0
        &p2 = 0x556cd1f41eb0
        p1 = d1f41eb0, p2 = d1f41eb0
    */

    for (int i = 0; i < 10; i++) {
        std::thread t(constructorSingleton, i);
        t.detach();
    }
    /*
    result:
        构造一个Singleton对象
        ptr of single: 0x7f2b58000f70, i = 0
        ptr of single: 0x7f2b58000f70, i = 2
        ptr of single: 0x7f2b58000f70, i = 3
        造一个Singleton对象
        ptr of single: 0x7f2b50000b60, i = 4
        ptr of single: 0x7f2b58000f90, i = 7
        ptr of single: 0x7f2b58000f90, i = 5
        构构造一个Singleton对象
        ptr of single: 0x7f2b40000b60, i = 6
        ptr of single: 0x7f2b50000b60
        ptr of single: 0x7f2b58000f90, i = 7
        构造一个Singleton对象
        ptr of single: 0x7f2b40000b60, i = 8
        ptr of single: 0x7f2b40000b60, i = 9
    可以看到存在多个不同的Singleton对象。
    解决方案：double check
    */

    getchar();



    return 0;
}



