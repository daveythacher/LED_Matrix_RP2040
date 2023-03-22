#!/bin/bash

sudo apt update
sudo apt install -y cmake git gcc-arm-none-eabi gcc g++ build-essential groovy python3 doxygen graphviz

git submodule update --init
cd LED_Matrix/lib/pico-sdk/
git submodule update 
