cmake_minimum_required(VERSION 3.5)
project(base_thread_cpp17)

set(CMAKE_CXX_STANDARD 17)

find_package(Threads REQUIRED)

add_executable(${PROJECT_NAME} 115base_thread_c++17.cpp)

target_link_libraries(${PROJECT_NAME} Threads::Threads tbb)