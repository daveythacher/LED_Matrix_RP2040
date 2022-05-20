/* 
 * File:   serial.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#include <stdint.h>
#include "pico/multicore.h"
//#include "hardware/dma.h"
#include "SPWM/config.h"
//#include "serial_spi/serial_spi.h"

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

#if 0
typedef struct {
    uint64_t marker;
    test buffer;
} serial_type;

static serial_type buffers[2];
static volatile bool isReady;
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
    extern void work();
    multicore_launch_core1(work);
    
    isReady = false;
    serial_dma_chan = dma_claim_unused_channel(true);
    serial_spi_start(&callback, serial_dma_chan, pio1, 0); 
}

#endif

