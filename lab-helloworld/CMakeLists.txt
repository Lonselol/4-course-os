cmake_minimum_required(VERSION 3.10)

project(lab-helloworld)

set(CMAKE_CXX_STANDARD 17)

if(EXISTS "${CMAKE_BINARY_DIR}/lab-helloworld")
    file(REMOVE "${CMAKE_BINARY_DIR}/lab-helloworld")
endif()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")

add_executable(lab-helloworld main.cpp)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT lab-helloworld)