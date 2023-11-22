#include <string>
#include <iostream>
#include "DynOBJ.h"

class Api
{
public:
    virtual void test(std::string s) = 0;
protected:
    Api() {}
};


class ImpleOne : public Api {
public:
    void test(std::string s) override {
        std::cout << "ImpleOne::test() " << s << std::endl;    
    }
};


class ImpleTwo : public Api {
public:
    void test(std::string s) override {
        std::cout << "ImpleTwo::test() " << s << std::endl;    
    }
};


class Factory
{
public:
    static Api* createApi(int type)
    {
        Api* pApi = nullptr;
        if (type == 1)
        {
            pApi = new ImpleOne();
        }
        if (type == 2)
        {
            pApi = new ImpleTwo();
        }
        return pApi;
    }

    static Api* createApi()
    {
        return new ImpleOne();
    }
};


/**
 * 使用场景：
 * 传入参数1，可以实现从数据库读入的功能
 * 传入参数2，可以实现从文本文件读入的功能
*/

int main1(void)
{
    Api* pApi = Factory::createApi(1);  // 客户端，还是知道工厂的细节
    pApi->test("SimpleFactor");

    Api* pApiExt = Factory::createApi();
    pApiExt->test("SimpleFactor");
    return 0;
}

// 实现了客户端调用和ImplOne，ImplTwo的解耦合。
// Factory类实现了变化隔离。


// 问题的需求：
/*
static Api* createApi()
{
    return new ImpleOne();
}

能否做到完全解耦合，以上方法灵活实现，想要ImpleOne就返回ImplOne，想要ImpleTwo就返回ImplTwo。

实现方法：动态创建
*/


// class ImpleTwoHelper {
// public:
//     ImpleTwoHelper() {
//         CObjectFactory::registerClass("ImpleTwo", ImpleTwoHelper::createObjFunc);
//     }
//     static void* createObjFunc() {
//         return new ImpleTwo;
//     }
// };
REG_CLASS(ImpleOne);
REG_CLASS(ImpleTwo);

int main2(int argc, char* argv[])
{
    // ImpleTwo* pTwo = static_cast<ImpleTwo*>(ImpleTwoHelper::createObjFunc());
    // pTwo->test("SimpleFactor");

    ImpleTwo* pTwo = static_cast<ImpleTwo*>(CObjectFactory::createObject("ImpleTwo"));
    pTwo->test("RegClass");
    ImpleOne* pOne = static_cast<ImpleOne*>(CObjectFactory::createObject("ImpleOne"));
    pOne->test("RegClass");
    return 0;
}


class AutoFactory
{
public:
    static Api* createApi() {
        Api* pApi = nullptr;
        pApi = static_cast<Api*>(CObjectFactory::createObject("ImpleTwo"));
        return pApi;
    }
};

int main(int argc, char* argv[])
{
    Api* pApi = AutoFactory::createApi();
    pApi->test("AutoFactory");
    return 0;
}