@echo off
set LAB_DIR=%1
if "%LAB_DIR%"=="" (
    echo Error: LAB_DIR is not specified.
    exit /b 1
)

git pull origin main

set BUILD_DIR=build\%LAB_DIR%
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
cd "%BUILD_DIR%"

cmake -G "MinGW Makefiles" -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ -DCMAKE_BUILD_TYPE=Debug ..\..\%LAB_DIR%
cmake --build . --config Debug

cd ..\..\..
