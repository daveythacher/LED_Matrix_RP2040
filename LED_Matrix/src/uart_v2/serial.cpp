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
#include "Serial/serial_uart_v2/serial_uart.h"

static packet buffers[2];
static volatile uint8_t buffer = 0;
static int serial_dma_chan;

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

static void __not_in_flash_func(callback)(uint8_t **buf, uint16_t *len) {
    *buf = (uint8_t *) &buffers[(buffer + 1) % 2];
    *len = sizeof(packet);
    buffer = (buffer + 1) % 2;
}

void __not_in_flash_func(serial_dma_isr)() {
    serial_uart_isr();
}

void serial_start() {
    multicore_launch_core1(work);
    
    serial_dma_chan = dma_claim_unused_channel(true);
    serial_uart_start(&callback, serial_dma_chan); 
}

