/* 
 * File:   main.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#include "Matrix/matrix.h"
#include "Serial/serial.h"
#include "ISR/isr.h"

int main() {
    matrix_start();
    serial_start();
    isr_start();
    
    while (1) {
        serial_task();
    }
}

