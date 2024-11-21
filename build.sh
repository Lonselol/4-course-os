set -e

git pull origin main  # Обновление кода из Git

export CC=gcc
export CXX=g++

mkdir -p build
cd build

cmake -DCMAKE_BUILD_TYPE=Debug ..

cmake --build . --config Debug

cd ..