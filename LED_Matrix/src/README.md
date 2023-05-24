# Overview
This folder contains the application binaries. Configuration is handled by the preprocessor and CMake workflow using values from xml configuration file. (This is handled by groovy and bash scripts.)

This calls the setup routines for interrupts, serial algorithms and matrix algorithms. These are standardized in this project and mostly boiler plate. This is implemented in the lib folder for the matrix, serial, multiplex, etc. algorithms.
