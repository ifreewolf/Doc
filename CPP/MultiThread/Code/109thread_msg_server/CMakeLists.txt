cmake_minimum_required(VERSION 3.5)
project(thread_msg_server)

set(CMAKE_CXX_STANDARD 14)

include_directories(
    ./include/
)

find_package(Threads REQUIRED)

add_executable(${PROJECT_NAME} 
    109thread_msg_server.cpp
    ./src/xthread.cpp
    ./src/xmsg_server.cpp
)

target_link_libraries(${PROJECT_NAME} Threads::Threads)