#include <thread>
#include <iostream>
#include <string>


class TestLambda
{
public:
    void Start()
    {
        std::thread th([this]() {std::cout << "name = " << name << std::endl; });
        th.join();
    }
    std::string name = "test lambda";
};

int main(int argc, char* argv[])
{
    std::thread th([](int i) { std::cout << "test lambda " << i << std::endl; }, 123);
    th.join();

    TestLambda test;
    test.Start();

    return 0;
}