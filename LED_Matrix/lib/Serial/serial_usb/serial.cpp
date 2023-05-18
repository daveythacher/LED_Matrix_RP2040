/* 
 * File:   serial.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#include <stdint.h>
#include "pico/multicore.h"
#include "hardware/dma.h"
#include "Matrix/config.h"
#include "Matrix/matrix.h"
#include "Serial/serial_usb/serial_usb.h"

static packet buffers[2];
static volatile uint8_t buffer = 0;
static int serial_dma_chan[2];

void serial_task() {  
#ifdef TEST
    for (int x = 0; x < COLUMNS; x++) {
        for (int y = 0; y < (2 * MULTIPLEX); y++) {
            if ((x % (2 * MULTIPLEX)) == y) {
                buffers[buffer].payload.data[y][x][0] = 0;
                buffers[buffer].payload.data[y][x][1] = 0;
                buffers[buffer].payload.data[y][x][2] = 0;
            }
            else {
                buffers[buffer].payload.data[y][x][0] = 0xFF;
                buffers[buffer].payload.data[y][x][1] = 0xFF;
                buffers[buffer].payload.data[y][x][2] = 0xFF;
            }
        }
    }
    
    buffer = (buffer + 1) % 2;
    process((void *) &buffers[(buffer + 1) % 2]);
#endif
}

// Not thread safe, reentrant, etc.
static void __not_in_flash_func(callback)(uint8_t **buf, uint16_t *len, uint8_t num) {
    static uint32_t count = serial_get_chunk_count();
    static uint32_t line = 0;
    num %= serial_get_chan_count();

    *buf = (uint8_t *) &buffers[(buffer + 1) % 2].mem[(line * serial_get_chan_count()) + num];
    *len = 64;

    if (num == serial_get_chan_count())     // This will always be called in order
        ++line;

    if (--count == 0) {
        buffer = (buffer + 1) % 2;
        process((void *) &buffers[(buffer + 1) % 2]);
        count = serial_get_chunk_count();
        line = 0;
    }
}

void __not_in_flash_func(serial_dma_isr)() {
    serial_usb_isr();
}

void serial_start() {
    multicore_launch_core1(work);
    
    serial_dma_chan[0] = dma_claim_unused_channel(true);
    serial_dma_chan[1] = dma_claim_unused_channel(true);
    serial_usb_start(&callback, serial_dma_chan[0], serial_dma_chan[1]); 
}

uint32_t serial_get_chan_count() {
    return std::min(sizeof(packet) / 64, (unsigned int) 15);
}

uint32_t serial_get_chunk_count() {
    return sizeof(packet) / 64;
}
