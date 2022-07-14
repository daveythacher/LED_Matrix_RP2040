/* 
 * File:   serial.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#include <stdint.h>
#include "pico/multicore.h"
#include "hardware/dma.h"
#include "BCM/config.h"
#include "serial_uart/serial_uart.h"

struct serial_type {
    uint8_t brightness;
    test buffer;
};

static volatile bool isReady;
static serial_type buffers[2];
static uint8_t buffer = 0;
static int serial_dma_chan;

void serial_task() {  
    if (isReady) {    
        extern void set_brightness(uint8_t);
        set_brightness(buffers[(buffer + 1) % 2].brightness);
        multicore_fifo_push_blocking((uint32_t) &buffers[(buffer + 1) % 2].buffer);
        isReady = false;
        serial_uart_print("Received\n");
    }
}

static void __not_in_flash_func(callback)(uint8_t **buf, uint16_t *len) {
    *buf = (uint8_t *) &buffers[(buffer + 1) % 2];
    *len = sizeof(serial_type);
    isReady = true;
    buffer = (buffer + 1) % 2;
}

void __not_in_flash_func(serial_dma_isr)() {
    serial_uart_isr();
}

void serial_start() {
    extern void work();
    multicore_launch_core1(work);
    
    isReady = false;
    serial_dma_chan = dma_claim_unused_channel(true);
    serial_uart_start(&callback, serial_dma_chan); 
}

