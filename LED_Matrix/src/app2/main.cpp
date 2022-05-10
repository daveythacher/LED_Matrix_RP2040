/* 
 * File:   main.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#include "pico/stdlib.h"

extern void matrix_start();
extern void serial_start();
extern void isr_start();
extern void serial_task();

int main() {
    stdio_init_all();
    matrix_start();
    serial_start();
    isr_start();
    
    while (1) {
        serial_task();
    }
}

