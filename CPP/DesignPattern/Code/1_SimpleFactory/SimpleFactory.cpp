#include <iostream>
#include <string>

// 没有严格的进行h和cpp的分离
// 对象间结构

class Api {
public:
    virtual void test(std::string s) = 0;
protected:
    Api() {}    // 屏蔽构造函数，体现接口
};

class Impl : public Api {
public:
    void test(std::string s) override {
        std::cout << "Impl::test() " << s << std::endl;
    }
};


// 测试用的客户端
int main(int argc, char *argv[])
{
    Api* pApi = new Impl();
    pApi->test("Hello");

    return 0;
}