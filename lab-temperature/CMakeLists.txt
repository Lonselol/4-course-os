cmake_minimum_required(VERSION 3.10)

project(lab-temperature)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

if(EXISTS "${CMAKE_BINARY_DIR}/lab-temperature")
    file(REMOVE "${CMAKE_BINARY_DIR}/lab-temperature")
endif()

find_package(Boost REQUIRED)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    message(STATUS "Boost найден: ${Boost_VERSION}")
else()
    message(FATAL_ERROR "Boost не найден. Убедитесь, что библиотека установлена.")
endif()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")

add_executable(lab-temperature main.cpp)
target_link_libraries(lab-temperature ${Boost_LIBRARIES})
