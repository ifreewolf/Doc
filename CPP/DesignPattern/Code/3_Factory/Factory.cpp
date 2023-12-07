#include <iostream>
#include <string>

class ExportFileApi
{
public:
    virtual bool exportData(std::string data) = 0;
protected:
    ExportFileApi(){}
};

// 具体化的子类
class ExportTxtFile : public ExportFileApi
{
public:
    bool exportData(std::string data) {
        std::cout << "正在导出数据" << data << "到csv文件" << std::endl;
    }
};

// 生成数据到数据库
class ExportDB : public ExportFileApi
{
public:
    bool exportData(std::string data) {
        std::cout << "正在导出数据到" << data << "到数据库" << std::endl;
    }
};

// 不让main函数知道有ExportDB和ExportTxtFile这两个类
// 实现一个ExportOperate，这个叫导出数据的业务功能对象
class ExportOperate // 这个也是接口
{
public:
    bool exportData(std::string data) {
        ExportFileApi* pApi = factoryMethod();
        return pApi->exportData(data);
    }
protected:
    virtual ExportFileApi* factoryMethod() = 0;
};

// 具体的实现对象，完成导出工作
class ExportTxtFileOperate : public ExportOperate
{
protected:
    ExportFileApi* factoryMethod() {
        return new ExportTxtFile;
    }
};

class ExportDBOperate : public ExportOperate
{
protected:
    ExportFileApi* factoryMethod() {
        return new ExportDB;
    }
};

// 开放ExportTxtFileOperate和ExportDBOperate


int main(int argc, char* argv[])
{
    ExportOperate* pOperate = new ExportTxtFileOperate();
    pOperate->exportData("HaHa");
    return 0;
}