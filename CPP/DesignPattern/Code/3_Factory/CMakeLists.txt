cmake_minimum_required(VERSION 3.5)
project(factory)

set(CMAKE_CXX_STANDARD 14)

add_executable(${PROJECT_NAME} Factory.cpp)

add_executable(iocTest IOC.cpp)
