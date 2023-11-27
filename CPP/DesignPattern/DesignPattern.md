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
> 在这种设计中，如果添加一个新的派生类，客户端不需要知道派生类的具体实现，通过传递指定的type，即可获得指向新派生类的对象指针。Factory需要添加对新派生类的支持，根据指定的type创建一个指向新派生类的对象。这种设计基本实现了“接口不变，内部实现的变化“的原则。
>
> 使用场景：传入参数1，可以实现从数据库读入的功能；传入参数2，可以实现从文本文件读入的功能。新添加一个修改的功能，客户端只需要做简单的修改，createF()传入3，即可实现修改数据库的功能，由Factory和ImplThree去做具体实现。隔离了变化。
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
    Api* pApi = Factory::createApi(1);  // 客户端，还是知道工厂的细节
    pApi->test("SimpleFactor");
    return 0;
}

// 实现了客户端调用和ImplOne，ImplTwo的解耦合。
// Factory类实现了变化隔离，但
```

以上工厂设计实现了主程序与具体实现类的解耦合，但不是完全的解耦合，因为工厂类和具体实现类之间还存在依赖关系，客户端还是需要知道工厂的细节。

<B>需求:</B> 能否做到完全解耦合，以上方法灵活实现，想要ImpleOne就返回ImplOne，想要ImpleTwo就返回ImplTwo。就像下面一样：

```cpp
static Api* createApi()
{
    return new ImpleOne();  // ImplOne, ImplTwo? 想要返回什么就返回什么？
}
```

<B>实现方法：动态创建</B>

实现流程：

1. 设计工厂类

```cpp
typedef void* (*Constructor) ();    // 仿函数

class CObjectFactory
{
public:
    static void registerClass(std::string className, Constructor constructor) {
        constructors()[className] = constructor;
    }

    static void* createObject(const std::string& className) {
        Constructor constructor = nullptr;
        if (constructors().find(className) != constructors().end()) {
            constructor = constructors().find(className)->second;
        }
        if (constructor == nullptr)
            return nullptr;
        return (*constructor)();
    }
private:
    // string->key：动态创建的类的类名，value是构建
    inline static std::map<std::string, Constructor>& constructors() {
        static std::map<std::string, Constructor> instance;
        return instance;
    }
};
```

> 以上设计一个工厂类CObjectFactory，通过registerClass可以注册新的实现类，通过createObject可以创建需要的派生类对象。

再来看看如何注册新的派生类：

```cpp
class ImpleTwoHelper {
public:
    ImpleTwoHelper() {
        CObjectFactory::registerClass("ImpleTwo", ImpleTwoHelper::createObjFunc);
    }
    static void* createObjFunc() {
        return new ImpleTwo;
    }
};

class ImpleOneHelper {
public:
    ImpleOneHelper() {
        CObjectFactory::registerClass("ImpleOne", ImpleOneHelper::createObjFunc);
    }
    static void* createObjFunc() {
        return new ImpleOne;
    }
};
```

上面是一个派生类的创建辅助类，这个程序由新建派生类的时候实现，它的构造函数调用了工厂类的注册函数来将该派生类注册到工厂类的map中。


客户端使用：

```cpp
ImpleTwo* pTwo = static_cast<ImpleTwo*>(ImpleTwoHelper::createObjFunc());
pTwo->test("SimpleFactor");

ImpleOneHelper impleOneHelper;
ImpleTwoHelper impleTwoHelper;

ImpleTwo* pTwo = static_cast<ImpleTwo*>(CObjectFactory::createObject("ImpleTwo"));
pTwo->test("RegClass");
ImpleOne* pOne = static_cast<ImpleOne*>(CObjectFactory::createObject("ImpleOne"));
pOne->test("RegClass");

// 上述两种使用方式都可以通过工厂获得派生类对象，需要注意：派生类的Helper以及派生类都由派生类创建者自己完成，这样就可以实现工厂与客户端完全解耦。
```

上述设计，已经完成了客户端与工厂的完全解耦，但我们可以设计的更方便一些，使用宏来代替派生类的Helper类，这样派生类创建者直接省了一个Helper类，客户端使用也方便了：

```cpp
#define REG_CLASS(class_name) \
class class_name##Helper { \
public: \
    class_name##Helper() { \
        CObjectFactory::registerClass(#class_name, class_name##Helper::createObjFunc); \
    } \
    static void* createObjFunc() { \
        return new class_name; \
    }\
}; \
class_name##Helper class_name##helper;
```

客户端使用：

```cpp
REG_CLASS(ImpleOne);
REG_CLASS(ImpleTwo);

int main2(int argc, char* argv[])
{
    ImpleTwo* pTwo = static_cast<ImpleTwo*>(CObjectFactory::createObject("ImpleTwo"));
    pTwo->test("RegClass");
    ImpleOne* pOne = static_cast<ImpleOne*>(CObjectFactory::createObject("ImpleOne"));
    pOne->test("RegClass");
    return 0;
}
```

通过使用宏，已经完成了工厂类与客户端的完全解耦合，但是还未能实现客户端与派生类之间的完全解耦合，因为客户端需要知道派生类创建者创建的派生类对象：

```cpp
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
```

> 上面的设计已经完成实现了客户端和派生类之间的完全解耦合，和工厂也实现了完全解耦合。但是有一个疑惑，这样设计的话，AutoFactory只能让客户端获取一个派生类对象，而无法获取多个派生类对象，如果需要获取多个派生类对象，那么就需要在客户端创建多个AutoFactory类，这样就违背了设计。
> 可以看到AutoFactory是通过一个字符串来确定获取的派生类对象，那么可以使用配置文件的方式来动态修改获取的派生类对象，这样就可以实现获取多个派生类对象。

---


# 二. 创建型模式之简单工厂