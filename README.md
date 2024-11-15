# 4-course-os
 
Для создания кэша cmake:
cd build
cmake ..

Для того чтобы собраться проект в build
cmake --build build

Для конкретной лабы
cmake --build build . --target lab-lib

build.cmd
@echo off
git pull origin main  # Обновление кода из Git
if not exist build mkdir build
cd build
cmake ..
cd ..
cmake --build build.

build.sh
#!/bin/bash
git pull origin main  # Обновление кода из Git
mkdir -p build
cd build
cmake ..
cd ..
make
