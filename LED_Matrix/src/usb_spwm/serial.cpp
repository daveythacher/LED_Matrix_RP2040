/* 
 * File:   serial.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#include <stdint.h>
#include "pico/multicore.h"
#include "SPWM/config.h"

//static volatile bool isReady;
//static test buffers[2];
//static uint8_t buffer = 0;

// This could be UART/RS-485, SPI, Ethernet, Etc.
//  If Ethernet it would be interesting to know if a webserver is possible.
void serial_start() {
    extern void work();
    multicore_launch_core1(work);
    // TODO:
    
    /*while (1) {
        if (isReady) {
            multicore_fifo_push_blocking((uint32_t) &buffers[(buffer + 1) % 2]);
            isReady = false;
        }
    }*/
}

