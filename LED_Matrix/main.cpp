/* 
 * File:   main.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */
 
 #include "FreeRTOS.h"
#include "task.h"

extern void matrix_start();
extern void serial_start();

int main() {
    matrix_start();
    serial_start();
    vTaskStartScheduler();
}

