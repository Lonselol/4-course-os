cmake_minimum_required(VERSION 3.10)
project(4-course-os)

if(NOT DEFINED LAB_FOLDER)
    message(FATAL_ERROR "LAB_FOLDER не задан. Укажите папку проекта.")
endif()

message(STATUS "Собирается лаборатория: ${LAB_FOLDER}")

if(EXISTS "${CMAKE_SOURCE_DIR}/${LAB_FOLDER}")
    add_subdirectory(${LAB_FOLDER})
else()
    message(FATAL_ERROR "Папка проекта ${LAB_FOLDER} не найдена.")
endif()
