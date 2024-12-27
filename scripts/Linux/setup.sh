#!/bin/bash

sudo apt update

# Install compiler
sudo apt install -y cmake git gcc-arm-none-eabi libnewlib-arm-none-eabi libstdc++-arm-none-eabi-newlib gcc g++ build-essential

# Install doxygen
sudo apt install -y python3 doxygen graphviz