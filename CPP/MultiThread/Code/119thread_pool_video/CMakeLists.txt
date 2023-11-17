cmake_minimum_required(VERSION 3.5)
project(thread_pool_video)

set(CMAKE_CXX_STANDARD 17)

find_package(Threads REQUIRED)

include_directories(
    ./include/
)

add_executable(${PROJECT_NAME}
    119thread_pool_video.cpp
    ./src/xthread_pool.cpp
)

target_link_libraries(${PROJECT_NAME} Threads::Threads)