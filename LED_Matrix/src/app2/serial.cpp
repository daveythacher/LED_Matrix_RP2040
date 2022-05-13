/* 
 * File:   serial.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#include <stdint.h>
#include "pico/multicore.h"
#include "hardware/dma.h"
#include "SPWM/config.h"
#include "serial_spi/serial_spi.h"

static volatile bool isReady;
static test buffers[2];
static uint8_t buffer = 0;

static int serial_dma_chan;

void serial_task() {

    /*for (int x = 0; x < COLUMNS; x++) {
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
    buffer = (buffer + 1) % 2;*/
    
    if (isReady) {
        multicore_fifo_push_blocking((uint32_t) &buffers[buffer]);
        buffer = (buffer + 1) % 2;
        isReady = false;
    }
}

void callback(uint8_t **buf, uint16_t *len) {
    *buf = (uint8_t *) buffers[(buffer + 1) % 2];
    *len = sizeof(test);
    isReady = true;
}

void serial_dma_isr() {
    serial_spi_isr();
}

void serial_start() {
    extern void work();
    multicore_launch_core1(work);
    
    isReady = false;
    serial_dma_chan = dma_claim_unused_channel(true);
    serial_spi_start(&callback, serial_dma_chan, pio1, 0); 
}

