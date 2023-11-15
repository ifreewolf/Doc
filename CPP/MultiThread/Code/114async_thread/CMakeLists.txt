cmake_minimum_required(VERSION 3.5)
project(async_thread)

set(CMAKE_CXX_STANDARD 14)

find_package(Threads REQUIRED)

add_executable(${PROJECT_NAME} 114async_thread.cpp)

target_link_libraries(${PROJECT_NAME} Threads::Threads)