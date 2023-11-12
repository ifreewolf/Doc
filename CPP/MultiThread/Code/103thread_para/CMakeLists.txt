cmake_minimum_required(VERSION 3.1)

project(thread_para)

set(CMAKE_CXX_STANDARD 11)

find_package(Threads REQUIRED)

add_executable(thread_para 103thread_para.cpp)

target_link_libraries(thread_para Threads::Threads)