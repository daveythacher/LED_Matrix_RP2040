/* 
 * File:   main.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */
 
 #include "FreeRTOS.h"
#include "task.h"
#include "matrix.h"
#include "serial.h"

int main() {
    matrix_start();
    serial_start();
    vTaskStartScheduler();
}

