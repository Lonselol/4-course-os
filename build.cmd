@echo off
git pull origin main

:: Очистка предыдущей сборки
if exist build rmdir /s /q build
mkdir build
cd build

:: Указание генератора и компиляторов
cmake -G "MinGW Makefiles" -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ -DCMAKE_BUILD_TYPE=Debug ..

:: Сборка
cmake --build . --config Debug

cd ..
