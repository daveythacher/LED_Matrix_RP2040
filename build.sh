#!/bin/sh
git submodule update --init
cd pico-sdk/
git submodule update --init
cd ..
DIR=$PWD
export PICO_SDK_PATH=$PWD/pico-sdk
cd LED_Matrix
mkdir build
cp ../pico-sdk/external/pico_sdk_import.cmake .
cd build
cmake ..
make -j $(($(nproc) * 2))
if [ $? -eq 0 ]; then
    echo "Binary output:"
    ls -la $DIR/LED_Matrix/build/led.*
fi
