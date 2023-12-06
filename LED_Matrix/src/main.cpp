/* 
 * File:   main.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "pico/stdio_usb.h"
#include "pico/multicore.h"
#include "Matrix/matrix.h"
#include "Serial/serial.h"
#include "ISR/isr.h"

void __not_in_flash_func(loop)() {
    while (1) {
        serial_task();
    }
}

int main() {
    stdio_usb_init();
    matrix_start();
    serial_start();
    isr_start();
    loop();
}

