#include <thread>
#include <iostream>
#include <string>

class Para
{
public:
    Para() { std::cout << "Create Para" << std::endl; }
    Para(const Para& p) { std::cout << "Copy Para" << std::endl; this->name = p.name; }  // 拷贝构造函数
    ~Para() { std::cout << "Drop Para " << std::endl; }
    std::string name;
};


void ThreadMain(int p1, float p2, std::string str, Para p4)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(100));    // 延时 100ms，等待大括号结束，float f2局部变量注销, 这个时候因为是拷贝的参数，所以不会有影响
    std::cout << "ThreadMain: " << p1 << " " << p2 << " " << str << " " << p4.name << std::endl;
}

void ThreadMainPtr(Para* p)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); 
    std::cout << "ThreadMainPtr: " << p->name << std::endl;
}

void ThreadMainRef(Para& p)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); 
    std::cout << "ThreadMainPtr: " << p.name << std::endl;
}

int main(int argc, char* argv[])
{
    {
        // 传递引用
        Para p;
        p.name = "test ref";
        std::thread th(ThreadMainRef, std::ref(p)); // 为什么要使用std::ref()，因为这里是一个回调，系统无法确定p的类型，如果是普通函数，则可以直接使用p
        th.join();
    }
    getchar();


    {
        // 传递线程指针
        Para p;
        p.name = "test ThreadMainPtr name";
        std::thread th(ThreadMainPtr, &p);  // 错误，线程访问的p空间会提前释放
        th.detach();
    }
    // Para 已经释放了
    // ThreadMainPtr: �        �Vtr name // 乱码，
    getchar();

    {
        Para p;
        p.name = "test ThreadMainPtr name";
        std::thread th(ThreadMainPtr, &p);
        th.join();
    }

    getchar();



    std::thread th;
    {
        float f2 = 12.1f;
        Para p;
        p.name = "test Para class";
        std::cout << "begin create thread" << std::endl;
        // 所有参数据做复制
        th = std::thread(ThreadMain, 101, f2, "test string para", p);   // 在这里参数的传递是复制
        std::cout << "end create thread" << std::endl;
    }
    std::cout << "end bracket" << std::endl;
    th.join();
    std::cout << "end main" << std::endl;

    /*
        三次拷贝：
            1. main函数里面，大括号内，把声明的Para对象p给std::thread()构造函数形参时，会产生一次拷贝；
            2. 回调的时候，会将std::thread()函数的p拷贝到ThreadMain()函数的形参p4
            3. 还有一次是什么？？？？
            我这里出现了1次构造，3次拷贝，4次析构
    */

    return 0;
}