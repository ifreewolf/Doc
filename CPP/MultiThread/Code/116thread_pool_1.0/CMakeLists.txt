cmake_minimum_required(VERSION 3.5)
project(xthread_pool_1.0)

set(CMAKE_CXX_STANDARD 11)

find_package(Threads REQUIRED)

include_directories(
    ./include/
)

add_executable(${PROJECT_NAME}
    116thread_pool_1.0.cpp
    ./src/xthread_pool.cpp
)

target_link_libraries(${PROJECT_NAME} Threads::Threads)