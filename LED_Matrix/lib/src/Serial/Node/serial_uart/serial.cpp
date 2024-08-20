/* 
 * File:   serial.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#include "pico/multicore.h"
#include "Serial/Node/serial_uart/serial_uart.h"

namespace Serial {
    void __not_in_flash_func(task)() { 
        Serial::UART::uart_task();
    }

    void start() {        
        Serial::UART::uart_start(); 
    }
}
