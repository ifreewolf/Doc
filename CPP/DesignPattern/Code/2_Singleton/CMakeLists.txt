cmake_minimum_required(VERSION 3.1)
project(singleton)

set(CMAKE_CXX_STANDARD 11)

find_package(Threads REQUIRED)

add_executable(${PROJECT_NAME}
    Singleton.cpp
)

target_link_libraries(${PROJECT_NAME} Threads::Threads)


add_executable(cacheSingleton CacheAndSingleton.cpp)
add_executable(multiSingleton MultiSingleton.cpp)