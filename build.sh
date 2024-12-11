#!/bin/bash
set -e

LAB_DIR=$1  # Путь к папке с текущей лабораторной работой
if [ -z "$LAB_DIR" ]; then
  echo "Error: LAB_DIR is not specified."
  exit 1
fi

git pull origin main  # Обновление кода из Git

export CC=gcc
export CXX=g++

BUILD_DIR="build/$LAB_DIR"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

cmake -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ -DCMAKE_BUILD_TYPE=Debug "../../$LAB_DIR"
cmake --build . --config Debug

cd ../../
