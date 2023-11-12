cmake_minimum_required(VERSION 3.1)
project(first_thead)

set(CMAKE_CXX_STANDARD 11)

find_package(Threads REQUIRED)

add_executable(first_thread 101first_thread.cpp)

target_link_libraries(first_thread Threads::Threads)