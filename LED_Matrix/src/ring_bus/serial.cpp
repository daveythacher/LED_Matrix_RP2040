/* 
 * File:   serial.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#include <stdint.h>
#include "pico/multicore.h"
#include "hardware/dma.h"
#include "BCM/config.h"
#include "serial_ring/serial_ring.h"

typedef struct {
    uint32_t id;
    test buffer;
} serial_type;

static volatile bool isReady;
static serial_type buffers[2];
static uint8_t buffer = 0;

static int serial_dma_chan_rx;
static int serial_dma_chan_tx;

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
        multicore_fifo_push_blocking((uint32_t) &buffers[(buffer + 1) % 2].buffer);
        isReady = false;
    }
}

void callback(uint8_t **buf_rx, uint8_t **buf_tx, uint16_t *len) {
    *buf_rx = (uint8_t *) &buffers[(buffer + 1) % 2];   // Back buffer
    *buf_tx = (uint8_t *) &buffers[buffer];             // Front buffer
    *len = sizeof(serial_type);
    
    if (buffers[buffer].id == 0)                        // Previous Back buffer will be Front buffer now
        isReady = true;
    buffers[buffer].id--;                               // Decrement hop counter, we consumed a hop.
    buffer = (buffer + 1) % 2;                          // Swap buffers for next time
}

void serial_dma_isr() {
    serial_ring_isr();
}

void serial_start() {
    extern void work();
    multicore_launch_core1(work);
    
    isReady = false;
    serial_dma_chan_rx = dma_claim_unused_channel(true);
    serial_dma_chan_tx = dma_claim_unused_channel(true);
    serial_ring_start(&callback, serial_dma_chan_rx, serial_dma_chan_tx); 
}

