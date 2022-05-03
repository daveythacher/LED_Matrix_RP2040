#!/bin/sh
git submodule update --init
cd LED_Matrix/lib/pico-sdk/
git submodule update --init
cd ../FreeRTOS/FreeRTOS-Kernel/
git submodule update --init
cd ../../../..
DIR=$(pwd)
export PICO_SDK_PATH=$DIR/LED_Matrix/lib/pico-sdk
cd LED_Matrix
mkdir build
cp $DIR/LED_Matrix/lib/pico-sdk/external/pico_sdk_import.cmake .
cd build
cmake .. -DDEFINE_MULTIPLEX=16 -DDEFINE_MAX_RGB_LED_STEPS=500 -Dapp1=TRUE -Dapp2=TRUE -Dapp3=TRUE
make -j $(($(nproc) * 2)) #VERBOSE=1
if [ $? -eq 0 ]; then
    echo "Binary output:"
    ls -la $DIR/LED_Matrix/build/src/led_*.*
fi
