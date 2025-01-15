cmake_minimum_required(VERSION 3.10)

project(lab-temperature)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

if(EXISTS "${CMAKE_BINARY_DIR}/lab-temperature")
    file(REMOVE "${CMAKE_BINARY_DIR}/lab-temperature")
endif()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")

add_executable(lab-temperature-device-simulator device_simulator.cpp)
add_executable(lab-temperature lab_temperature.cpp)
