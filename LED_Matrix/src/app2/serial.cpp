/* 
 * File:   serial.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#include <stdint.h>
#include <stdio.h>
#include "pico/multicore.h"
#include "SPWM/config.h"

static volatile bool isReady;
static test buffers[2];
static uint8_t buffer = 0;

void serial_task() {

    for (int x = 0; x < COLUMNS; x++) {
        for (int y = 0; y < (2 * MULTIPLEX); y++) {
            if ((x % MULTIPLEX) == y) {
                buffers[buffer][y][x][0] = 0;
                buffers[buffer][y][x][1] = 0;
                buffers[buffer][y][x][2] = 0;
            }
            else {
                buffers[buffer][y][x][0] = 0xFF;
                buffers[buffer][y][x][1] = 0xFF;
                buffers[buffer][y][x][2] = 0xFF;
            }
        }
    }
    isReady = true;
    buffer = (buffer + 1) % 2;
    
    if (isReady) {
        printf("serial_task: yup...\n");
        multicore_fifo_push_blocking((uint32_t) &buffers[(buffer + 1) % 2]);
        isReady = false;
    }
}

void serial_start() {
    extern void work();
    multicore_launch_core1(work);
    // TODO:
}

