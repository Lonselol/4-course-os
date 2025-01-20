cmake_minimum_required(VERSION 3.10)
project(4-course-os)

if(NOT DEFINED LAB_DIR)
    message(FATAL_ERROR "LAB_DIR не задан. Укажите папку проекта.")
endif()

message(STATUS "Собирается лабораторная: ${LAB_DIR}")

if(EXISTS "${CMAKE_SOURCE_DIR}/${LAB_DIR}")
    add_subdirectory(${LAB_DIR})
else()
    message(FATAL_ERROR "Папка проекта ${LAB_DIR} не найдена.")
endif()
