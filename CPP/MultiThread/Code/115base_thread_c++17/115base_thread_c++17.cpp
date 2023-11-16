#include <thread>
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <execution>

static const char base16[] = "0123456789abcdef";

void Base16Encode(const unsigned char* data, int size, unsigned char* out)
{
    for (int i = 0; i < size; i++)
    {
        unsigned char d = data[i];
        // 0000 0000
        // 1234 5678 >>4 0000 1234
        // 1234 5678 & 0000 1111 0000 5678
        char a = base16[d >> 4];
        char b = base16[d & 0x0F];
        out[i * 2] = a;
        out[i*2 + 1] = b;
    }
}

// C++11 多核base16编码
void Base16EncodeThread(const std::vector<unsigned char>& data, std::vector<unsigned char>& out)
{
    int size = data.size();
    int th_count = std::thread::hardware_concurrency(); // 系统支持的线程核心数
    // std::cout << "hardware_concurrency: " << th_count << std::endl;
    // 切片数据
    int slice_count = size / th_count;
    if (size < th_count)   // 只切一片
    {
        th_count = 1;
        slice_count = size;
    }

    // 准备好线程
    std::vector<std::thread> ths;
    ths.resize(th_count);

    // 任务分配到各个线程
    for (int i = 0; i < th_count; i++)
    {
        // 1234 5678 9abc defg
        int offset = i * slice_count;
        int count = slice_count;

        // 最后一个
        if (th_count > 1 && i == th_count - 1)
        {
            count = slice_count + size % th_count;
        }
        // std::cout << offset << ":" << count << std::endl;

        ths[i] = std::thread(Base16Encode, data.data() + offset, count, out.data() + offset*2);
    }

    // 等待所有线程处理结束
    for (auto& th : ths)
    {
        th.join();
    }

}

int main(int argc, char* argv[])
{
    std::string test_data = "测试base16编码";
    unsigned char out[1024] = { 0 };
    Base16Encode((unsigned char*)test_data.data(), test_data.size(), out);
    std::cout << "base16: " << out << std::endl;

    // 初始化测试数据
    std::vector<unsigned char> in_data;
    in_data.resize(1024*1024*100);   // 20M
    // in_data.resize(100);   // 20M
    for (int i = 0; i < in_data.size(); i++)
    {
        in_data[i] = i % 256;
    }
    
    std::vector<unsigned char> out_data;
    out_data.resize(in_data.size()*2);

    // 测试单线程base16编码效率
    {
        std::vector<unsigned char> out_data;
        out_data.resize(in_data.size()*2);
        std::cout << "单线程base16编码 开始计算" << std::endl;
        auto start = std::chrono::system_clock::now();
        Base16Encode(in_data.data(), in_data.size(), out_data.data());
        auto end = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        // std::cout << out_data.data() << std::endl;
        std::cout << "编码：" << in_data.size() << " 字节数据花费 " << duration.count() << " 毫秒" << std::endl;
    }

    // 测试C++11 多线程base16编码效率
    {
        std::vector<unsigned char> out_data;
        out_data.resize(in_data.size()*2);
        std::cout << "C++11 多线程base16编码 开始计算" << std::endl;
        auto start = std::chrono::system_clock::now();
        Base16EncodeThread(in_data, out_data);
        auto end = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        // std::cout << out_data.data() << std::endl;
        std::cout << "编码：" << in_data.size() << " 字节数据花费 " << duration.count() << " 毫秒" << std::endl;
    }

    // 测试C++17 多线程base16编码效率
    {
        std::vector<unsigned char> out_data;
        out_data.resize(in_data.size()*2);
        std::cout << "C++17 多线程base16编码 开始计算" << std::endl;
        auto start = std::chrono::system_clock::now();
        unsigned char* idata = in_data.data();
        unsigned char* odata = out_data.data();
        // #include <execution>
        std::for_each(std::execution::par,  // 并行计算 多核
            in_data.begin(), in_data.end(), // 数据的开头和结尾地址
            [&](auto& d)
            {
                char a = base16[(d >> 4)];
                char b = base16[(d & 0x0f)];
                int index = &d - idata;    // 获得d在数组中的位置
                odata[index * 2] = a;
                odata[index * 2 + 1] = b;

            }
        );

        auto end = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        // std::cout << out_data.data() << std::endl;
        std::cout << "编码：" << in_data.size() << " 字节数据花费 " << duration.count() << " 毫秒" << std::endl;
    }
}