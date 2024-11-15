cmake_minimum_required(VERSION 3.10)

project(lab-helloworld)

set(CMAKE_CXX_STANDARD 17)

if(EXISTS "${CMAKE_BINARY_DIR}/lab-helloworld.exe")
    file(REMOVE "${CMAKE_BINARY_DIR}/lab-helloworld.exe")
endif()
add_executable(lab-helloworld main.cpp)