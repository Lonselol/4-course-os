cmake_minimum_required(VERSION 3.10)

project(lab-lib)

set(CMAKE_CXX_STANDARD 17)

if(EXISTS "${CMAKE_BINARY_DIR}/lab-lib")
    file(REMOVE "${CMAKE_BINARY_DIR}/lab-lib")
endif()
add_executable(lab-lib process_launcher.cpp)
