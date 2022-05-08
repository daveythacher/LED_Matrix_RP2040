/* 
 * File:   serial.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#include <stdint.h>
#include "pico/multicore.h"
#include "SPWM/config.h"

static volatile bool isReady;
static test buffers[2];
static uint8_t buffer = 0;

void serial_task() {
    if (isReady) {
        multicore_fifo_push_blocking((uint32_t) &buffers[(buffer + 1) % 2]);
        isReady = false;
    }
}

void serial_start() {
    extern void work();
    multicore_launch_core1(work);
    // TODO:
}

