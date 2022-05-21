/* 
 * File:   serial.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#include <stdint.h>
#include "pico/multicore.h"
#include "BCM/config.h"

extern "C" void usb_start();

static test buffers[2];
static uint8_t buffer = 0;

void serial_start() {
    extern void work();
    
    usb_start();
    multicore_launch_core1(work);
}

extern "C" void usb_receive(uint8_t *buf, uint16_t len) {
    uint8_t row = buf[0] >> 2;
    uint16_t col = ((buf[0] & 63) << 8) + buf[1];
    
    for (uint16_t i = 2; i < len; i+= 3) {
        buffers[buffer][row][col][0] = buf[i + 0];
        buffers[buffer][row][col][1] = buf[i + 1];
        buffers[buffer][row][col][2] = buf[i + 2];
        
        if (++col >= COLUMNS) {
            col = 0;
            if (++row >= (2 * MULTIPLEX)) {
                row = 0;
                multicore_fifo_push_blocking((uint32_t) &buffers[buffer]);
                buffer = (buffer + 1) % 2;
            }
        }
    }
}

