cmake_minimum_required(VERSION 3.5)
project(barrier_thread)

set(CMAKE_CXX_STANDARD 20)

find_package(Threads REQUIRED)

add_executable(${PROJECT_NAME} 120barrier_thread_c++20.cpp)

target_link_libraries(${PROJECT_NAME} Threads::Threads)
