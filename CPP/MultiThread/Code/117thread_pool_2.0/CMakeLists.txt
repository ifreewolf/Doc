cmake_minimum_required(VERSION 3.5)
project(xthread_pool_2.0)

set(CMAKE_CXX_STANDARD 17)

find_package(Threads REQUIRED)

include_directories(
    ./include/
)

add_executable(${PROJECT_NAME}
    117thread_pool_2.0.cpp
    ./src/xthread_pool.cpp
)

target_link_libraries(${PROJECT_NAME} Threads::Threads)