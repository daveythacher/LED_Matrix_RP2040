/* 
 * File:   main.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#include <stdio.h>
#include "pico/stdio.h"
#include "Matrix/config.h"
#include "Matrix/matrix.h"
#include "ISR/isr.h"
#include "Serial/serial.h"

int main() {
    stdio_init_all();
    matrix_start();
    serial_start();
    isr_start();
    
    printf("LED Matrix on RP2040\n");
    printf("Built on %s at %s\n", __DATE__, __TIME__);
    printf("Display size %dx%d\n", 2*MULTIPLEX, COLUMNS);
    printf("Target Refresh %dHz using %d PWM bits\n", MAX_REFRESH, PWM_bits);
    printf("This version is for standard shift register panels only!\n");
    printf("Traditional PWM is used and implemented using BCM\n");
    printf("Blanking time is set to %duS\n", BLANK_TIME);
    
    while (1) {
        serial_task();
    }
}

