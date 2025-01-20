cmake_minimum_required(VERSION 3.10)

project(lab-server)

set(CMAKE_CXX_STANDARD 17)

#sudo apt install sqlite3 libsqlite3-dev
if (WIN32)
    set(SQLite3_INCLUDE_DIR "C:/sqlite3")
    set(SQLite3_LIBRARY "C:/sqlite3/libsqlite3.a")
endif()

find_package(SQLite3 REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

if(EXISTS "${CMAKE_BINARY_DIR}/lab-server")
    file(REMOVE "${CMAKE_BINARY_DIR}/lab-server")
endif()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")

add_executable(lab-server main.cpp)

target_link_libraries(lab-server PRIVATE SQLite::SQLite3)

if (WIN32)
    target_link_libraries(lab-server PRIVATE ws2_32)
endif()

if (UNIX AND NOT APPLE)
    target_link_libraries(lab-server PRIVATE pthread)
endif()
