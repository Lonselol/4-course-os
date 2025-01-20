cmake_minimum_required(VERSION 3.10)

project(lab-qwt)

set(CMAKE_CXX_STANDARD 17)

if(EXISTS "${CMAKE_BINARY_DIR}/lab-qwt")
    file(REMOVE "${CMAKE_BINARY_DIR}/lab-qwt")
endif()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")

add_executable(lab-qwt main.cpp)
