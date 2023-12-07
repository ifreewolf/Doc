#include <string>
#include <map>
#include <memory>
#include <functional>
#include <iostream>

template<class T>
class IoCContainer
{
public:
    IoCContainer(void) {}
    ~IoCContainer()
    {

    }

    // 注册需要创建对象的构造函数，通过一个唯一的标识，以便以后查找
    template<class Drived>
    void RegisterType(std::string strKey) {
        std::function<T* ()> function = [] {return new Drived(); }
        RegisterType(strKey, function);
    }

    // 根据唯一的标识去查找对应的构造函数
    T* Resolve(std::string strKey) {
        if (m_createMap.find(strKey) == m_createMap.end())
            return nullptr;
        std::function<T* ()> function = m_createMap[strKey];
        return function();
    }

    // 创建智能指针
    std::shared_ptr<T> ResolveShared(std::string strKey) {
        T* ptr = Resolve(strKey);
        return std::shared_ptr<T>(ptr);
    }
private:
    void RegisterType(std::string strKey, std::function<T* ()> creator) {
        if (m_createMap.find(strKey) != m_createMap.end()) {
            throw std::invalid_argument("已经存在这个key了");
        }
        m_createMap.emplace(strKey, creator);
    }
private:
    std::map<std::string, std::function<T* ()>> m_createMap;
};