cmake_minimum_required(VERSION 3.1)

project(thread_lambda)

set(CMAKE_CXX_STANDARD 11)

find_package(Threads REQUIRED)

add_executable(${PROJECT_NAME} 105thread_lambda.cpp)

target_link_libraries(${PROJECT_NAME} Threads::Threads)