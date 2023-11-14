cmake_minimum_required(VERSION 3.5)
project(thread_future)

set(CMAKE_CXX_STANDARD 11)

find_package(Threads REQUIRED)

add_executable(${PROJECT_NAME} 112thread_future.cpp)

target_link_libraries(${PROJECT_NAME} Threads::Threads)