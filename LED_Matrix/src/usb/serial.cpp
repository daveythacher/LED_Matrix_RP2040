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

static constexpr uint32_t serial_chan_count = std::min(sizeof(packet) / 64, (unsigned int) 15);

static void __not_in_flash_func(callback)(uint8_t **buf, uint16_t *len, uint8_t num) {
    const uint32_t count = sizeof(packet) / (serial_chan_count * 64);
    static uint32_t counters[serial_chan_count] = { count };
    static uint32_t counter = count * serial_chan_count;
    num %= serial_chan_count;

    if (counters[num] == 0) 
        while (1);
    else
        counter--;

    *buf = (uint8_t *) &buffers[(buffer + 1) % 2].mem[(count - counters[num]) * serial_chan_count];
    *len = 64;
    counters[num]--;

    if (counter == 0) {
        buffer = (buffer + 1) % 2;
        for (uint8_t i = 0; i < serial_chan_count; i++)             // Terrible Design!?
            counters[i] = count;
        counter = count * serial_chan_count;
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
    return serial_chan_count;
}
