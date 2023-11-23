# 一. 创建型模式之简单工厂
---

- 学习简单工厂
- 一：简单工厂模式的介绍-定义、结构、参考实现、场景问题
- 二：简单工厂模式的典型疑问与优缺点评价
- 三：简单工厂模式的应用案例与思考

## 1.1 接口？ 什么是针对接口编程？
---

- 接口从语法层面上来说，是一种特殊的抽象类，是一个纯虚的类。从软件设计的意义上来说，我们通常用接口来定义实现类的外观，就相当于一份契约，根据外部应用需要的功能，约定了实现类应该要实现的功能。
- 软件开发中永恒的主题是“变化”，“只有变化才是永恒不变”，接口最重要的一个设计语义就是封装变化。所谓“封装变化”就是隔离变化。
- 从软件的整体结构上看，只要接口不变，内部实现的变化就不会影响到外部应用，从而使得系统更灵活，且有更好的扩展性和可维护性。

### 1.1.1 场景
---

- 比如，通常的，我们设计了一组API，一个程序需要使用这些API完成功能。我们如何规划API的设计？


### 1.1.2 简单工厂的定义

---

- 提供一个创建对象实例的功能，而无需关心其具体实现。被创建的类型可以是接口、抽象类，也可以是具体类。

<B>设计</B>

<div align=center>
    <img src="./UML/SimpleFactory.png" />
</div>

<B>分析</B>

> ImplOne和ImplTwo两个具体的实现类继承Api，Api是一个接口，定义了一个纯虚函数test()。Factory负责Api派生类的创建，根据不同的type创建不同的Api派生类，返回的对象是一个Api指针，指向Api的派生类。
>
> 在这种设计中，如果添加一个新的派生类，客户端不需要知道派生类的具体实现，通过传递指定的type，即可获得指向新派生类的对象指针。Factory需要添加对新派生类的支持，
> 后即可让客户端使用新的派生类方法，实现了“接口不变，内部实现的变化“的原则。
>
> 使用场景：传入参数1，可以实现从数据库读入的功能；传入参数2，可以实现从文本文件读入的功能。新添加一个修改的功能，客户端只需要做简单的修改，createF()传入3，即可实现修改数据库的功能，由Factory和ImplThree去做具体实现。隔离了变化。、
>
> 如果不使用简单工厂模式，那么客户端需要`Api* pApi = new ImpleThree()` 才能获得新的功能。
>
> 实现了客户端调用和ImplOne，ImplTwo的解耦合，Factory类实现了变化隔离。
>
> 以上的设计，客户端和工厂的唯一关联是需要通过输入不同的type值去得到不同的功能。是一个低耦合的设计，但不是完全解耦合。

<B>示例如下：</B>

```cpp
#include <string>
#include <iostream>

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


class Factory       // 负责实现类的创建
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
};


/**
 * 使用场景：
 * 传入参数1，可以实现从数据库读入的功能
 * 传入参数2，可以实现从文本文件读入的功能
*/

int main(void)
{
    Api* pApi = Factory::createApi(1);
    pApi->test("SimpleFactor");
    return 0;
}

// 实现了客户端调用和ImplOne，ImplTwo的解耦合。
// Factory类实现了变化隔离，但
```