/* 
 * File:   main.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#include "Serial/serial_uart/serial_uart.h"
#include "Matrix/config.h"
#include "Matrix/matrix.h"
#include "Serial/serial.h"
#include "ISR/isr.h"

int main() {
    matrix_start();
    serial_start();
    isr_start();
    
    serial_uart_printf("LED Matrix on RP2040\n");
    serial_uart_printf("Built on %s at %s\n", __DATE__, __TIME__);
    serial_uart_printf("Display size %dx%d\n", 2 * MULTIPLEX, COLUMNS);
    serial_uart_printf("Target Refresh %dHz using %d PWM bits\n", MAX_REFRESH, PWM_bits);
    serial_uart_printf("This version is for standard shift register panels only!\n");
    serial_uart_printf("Traditional PWM is used and implemented using BCM\n");
    serial_uart_printf("Blanking time is set to %duS\n", BLANK_TIME);
    
    while (1) {
        serial_task();
    }
}

