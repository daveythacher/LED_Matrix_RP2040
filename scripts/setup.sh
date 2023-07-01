#!/bin/bash

sudo apt update
sudo apt install -y cmake git gcc-arm-none-eabi libnewlib-arm-none-eabi libstdc++-arm-none-eabi-newlib gcc g++ build-essential groovy python3 doxygen graphviz ruby-full

sudo gem install cucumber
sudo gem install rspec-expectations

git submodule update --init
cd LED_Matrix/lib/pico-sdk/
git submodule update 
