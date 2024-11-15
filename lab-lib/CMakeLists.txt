cmake_minimum_required(VERSION 3.10)

project(lab-lib)

set(CMAKE_CXX_STANDARD 17)

add_executable(lab-lib test.cpp)
add_executable(lab-lib process_launcher.cpp)
