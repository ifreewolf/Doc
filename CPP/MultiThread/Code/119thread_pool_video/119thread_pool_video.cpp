#include "xthread_pool.h"
#include <iostream>

// 命令行视频转码工具
// ffmpeg 工具
// 用户输入 视频源 输出视频尺寸
// 在线程池中执行转码任务
// 转码任务调用ffmpeg
// ffmpeg -y -i test.mp4 -s 400x300 400.mp4 >log.txt 2>&1

int main(int argc, char* argv[])
{
    XThreadPool pool;
    pool.Init(16);
    pool.Start();

    return 0;
}