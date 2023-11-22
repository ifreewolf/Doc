cmake_minimum_required(VERSION 3.5)
project(SimpleFactory)

set(CMAKE_CXX_STANDARD 17)

include_directories(
    ./include/
)

add_executable(${PROJECT_NAME} SimpleFactory.cpp)
add_executable(SimpleFactoryRef SimpleFactoryRefactor.cpp ./include/DynOBJ.h)
