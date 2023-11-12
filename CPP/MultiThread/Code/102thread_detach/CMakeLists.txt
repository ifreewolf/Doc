cmake_minimum_required(VERSION 3.1)
project(thread_detach VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 11)

find_package(Threads REQUIRED)

add_executable(thread_detach 102thread_detach.cpp)

target_link_libraries(thread_detach Threads::Threads)