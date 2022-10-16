/* 
 * File:   serial.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#include <stdint.h>
#include <stdio.h>
#include "pico/multicore.h"
#include "Matrix/config.h"
#include "Matrix/matrix.h"

static packet buffers[2];
static uint8_t buffer = 0;

void serial_start() {    
    multicore_launch_core1(work);
}

void serial_task() {
    uint8_t *buf = (uint8_t *) &buffers[buffer];
   
    for (int i = sizeof(packet); i > 0; i--) {
        *buf = getc(stdin);
        buf++;
    }
        
    multicore_fifo_push_blocking((uint32_t) &buffers[buffer]);
    buffer = (buffer + 1) % 2;
    printf("Received\n");
}

