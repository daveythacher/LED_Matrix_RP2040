# Overview
This folder contains the application binaries. Configuration is handled by the preprocessor and CMake workflow.

This calls the setup routines for interrupts, serial algorithms and matrix algorithms. These are standardized in this project and mostly boiler plate. This is implemented in the lib folder for the matrix, serial, multiplex, etc. algorithms.


Note to self:
Gave RTOS both scratch banks. IO can compete against Multiplex. DMA should be promoted to allow PIO to move. DMA is IO singleton. Manage priority within DMA. CPU performance may be improved or degraded by RTOS. (Utilization vs bus contention.)
