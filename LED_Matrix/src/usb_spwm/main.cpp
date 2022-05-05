/* 
 * File:   main.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */
 
 #include "pico/stdlib.h"

extern void matrix_start();
extern void serial_start();
extern void isr_start();
extern "C" void usb_start();

int main() {
    usb_start();
    matrix_start();
    serial_start();
    isr_start();
    
    while (1) {
        tight_loop_contents();
    }
}

