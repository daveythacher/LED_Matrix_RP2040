/* 
 * File:   serial.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#include <stdint.h>
#include <stdio.h>
#include "pico/multicore.h"
#include "Matrix/BCM/config.h"
#include "Matrix/matrix.h"

static test buffers[2];
static uint8_t buffer = 0;

void serial_start() {    
    multicore_launch_core1(work);
}

static void usb_receive(uint8_t *buf, uint16_t len) {
    uint8_t row = 0;
    uint16_t col = 0;
    
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

void serial_task() {
   uint8_t buf[sizeof(test)];
   
    for (int i = sizeof(buf); i > 0; i--)
        buf[i] = getc(stdin);
        
   usb_receive(buf, sizeof(buf));
   printf("Received\n");
}

