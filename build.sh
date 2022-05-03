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
rm -rf build
mkdir build
cp $DIR/LED_Matrix/lib/pico-sdk/external/pico_sdk_import.cmake .
cd build
#cmake .. -DDEFINE_MULTIPLEX=8 -DDEFINE_MAX_RGB_LED_STEPS=500 -DDEFINE_MAX_REFRESH=200 -Dapp1=TRUE                           #Max resolution of 16x128 200Hz 18-bit color - Runs out of serial bandwidth
cmake .. -DDEFINE_MULTIPLEX=8 -DDEFINE_MAX_RGB_LED_STEPS=250 -DDEFINE_MAX_REFRESH=200 -Dapp1=TRUE                           #Max resolution of 16x128 400Hz 15-bit color - Near P6 refresh limit
#cmake .. -DDEFINE_MULTIPLEX=8 -DDEFINE_MAX_RGB_LED_STEPS=62 -DDEFINE_MAX_REFRESH=400 -Dapp2=TRUE                            #Max resolution of 16x128 400Hz 9-bit color - Code issue limits color depth

#cmake .. -DDEFINE_MULTIPLEX=16 -DDEFINE_MAX_RGB_LED_STEPS=500 -DDEFINE_MAX_REFRESH=200 -Dapp1=TRUE                          #Max resolution of 32x128 200Hz 15-bit color - Runs out of serial bandwidth
#cmake .. -DDEFINE_MULTIPLEX=16 -DDEFINE_MAX_RGB_LED_STEPS=500 -Dapp2=TRUE                                                   #Max resolution of 32x128 3.8kHz 15-bit color
#cmake .. -DDEFINE_MULTIPLEX=16 -DDEFINE_MAX_RGB_LED_STEPS=500 -Dapp3=TRUE                                                   #Max resolution of 32x256 3.8kHz 15-bit color - Runs out of memory

#cmake .. -DDEFINE_MULTIPLEX=32 -DDEFINE_MAX_RGB_LED_STEPS=500 -DDEFINE_MAX_REFRESH=200 -Dapp1=TRUE                          #Max resolution of 64x64 200Hz 12-bit color - Runs out of serial bandwidth
#cmake .. -DDEFINE_MULTIPLEX=32 -DDEFINE_MAX_RGB_LED_STEPS=500 -Dapp2=TRUE                                                   #Max resolution of 64x128 3.8kHz 12-bit color - Runs out of CPU
#cmake .. -DDEFINE_MULTIPLEX=32 -DDEFINE_MAX_RGB_LED_STEPS=500 -Dapp3=TRUE                                                   #Max resolution of 64x128 3.8kHz 12-bit color - Runs out of memory

#cmake .. -DDEFINE_MULTIPLEX=16 -DDEFINE_MAX_RGB_LED_STEPS=2000 -DDEFINE_MAX_REFRESH=200 -DDEFINE_COLUMNS=32 -Dapp1=TRUE     #Max resolution of 32x32 200Hz 21-bit color - Runs out of serial bandwidth
#cmake .. -DDEFINE_MULTIPLEX=2 -DDEFINE_MAX_RGB_LED_STEPS=250 -DDEFINE_COLUMNS=32 -Dapp2=TRUE                                #Max resolution of 4x32 3.8kHz 21-bit color - Runs out of memory (code issue)
#cmake .. -DDEFINE_MULTIPLEX=32 -DDEFINE_MAX_RGB_LED_STEPS=4000 -Dapp3=TRUE                                                  #Max resolution of 64x128 3.8kHz 21-bit color - Runs out of memory
make -j $(($(nproc) * 2)) #VERBOSE=1
if [ $? -eq 0 ]; then
    echo "Binary output:"
    ls -la $DIR/LED_Matrix/build/src/led_*.*
fi

# Future:
# Binary tool which picks firmware based on: MULTIPLEX, ROWS, COLUMNS, LED Driver and Row Driver. (All available on the panel.)
#   Binary tool can guess refresh based on LED driver and Row driver.
#   firmware can use default FPS, Max Refresh, Blanking time, LED color depth/steps, etc.
# Application is responsible for color order mapping, pixel mapping and multiplex mapping.
