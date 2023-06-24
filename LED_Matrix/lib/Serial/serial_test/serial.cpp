/* 
 * File:   serial.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#include <stdint.h>
#include "pico/multicore.h"
#include "Serial/config.h"
#include "Matrix/matrix.h"

static packet buffers[2];
static volatile uint8_t buffer = 0;

static void __not_in_flash_func(test_driver)() {
    for (int x = 0; x < COLUMNS; x++) {
        for (int y = 0; y < (2 * MULTIPLEX); y++) {
            if ((x % (2 * MULTIPLEX)) == y) {
                buffers[buffer].data[y][x][0] = 0;
                buffers[buffer].data[y][x][1] = 0;
                buffers[buffer].data[y][x][2] = 0;
            }
            else {
                buffers[buffer].data[y][x][0] = 0xFF;
                buffers[buffer].data[y][x][1] = 0xFF;
                buffers[buffer].data[y][x][2] = 0xFF;
            }
        }
    }
    
    buffer = (buffer + 1) % 2;   
    process((void *) &buffers[(buffer + 1) % 2]);
}

void __not_in_flash_func(serial_task)() {
    test_driver();
}

void __not_in_flash_func(serial_dma_isr)() {
    // Do nothing
}

void serial_start() {
    multicore_launch_core1(work);
}
