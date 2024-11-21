set -e

git pull origin main  # Обновление кода из Git

export CC=gcc
export CXX=g++

mkdir -p build
cd build

cmake -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ -DCMAKE_BUILD_TYPE=Debug ..

cmake --build . --config Debug

cd ..