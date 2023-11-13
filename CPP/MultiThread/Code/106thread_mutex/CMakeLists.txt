cmake_minimum_required(VERSION 3.1)

project(thread_mutex)

set(CMAKE_CXX_STANDARD 11)

find_package(Threads REQUIRED)

add_executable(${PROJECT_NAME} 106thread_mutex.cpp)

target_link_libraries(${PROJECT_NAME} Threads::Threads)