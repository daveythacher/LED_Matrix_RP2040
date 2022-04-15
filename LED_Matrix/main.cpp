/* 
 * File:   main.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */
 
 #include "FreeRTOS.h"
#include "task.h"
#include "pico/multicore.h"
#include "matrix.h"
#include "serial.h"

extern void work();

int main() {
    matrix_start();
    serial_start();
    multicore_launch_core1(work);
    vTaskStartScheduler();
}

