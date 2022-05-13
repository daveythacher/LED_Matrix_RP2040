/* 
 * File:   serial.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#include <stdint.h>
#include "pico/multicore.h"
#include "hardware/dma.h"
#include "config.h"
#include "serial_spi/serial_spi.h"

typedef struct {
    uint64_t marker;
    test buffer;
} serial_type;

static volatile bool isReady;
static serial_type buffers[2];
static uint8_t buffer = 0;

static int serial_dma_chan;

void serial_task() {
    
    // TODO:
    
    if (isReady) {
        multicore_fifo_push_blocking((uint32_t) &buffers[buffer].buffer);
        buffer = (buffer + 1) % 2;
        isReady = false;
    }
}

void callback(uint8_t **buf, uint16_t *len) {
    *buf = (uint8_t *) &buffers[(buffer + 1) % 2];
    *len = sizeof(serial_type);
    isReady = true;
}

void serial_dma_isr() {
    serial_spi_isr();
}

void serial_start() {
    // TODO:
    
    isReady = false;
    serial_dma_chan = dma_claim_unused_channel(true);
    serial_spi_start(&callback, serial_dma_chan, pio1, 0); 
}

