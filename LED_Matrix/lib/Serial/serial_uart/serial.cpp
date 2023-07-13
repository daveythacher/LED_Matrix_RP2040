/* 
 * File:   serial.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#include <stdint.h>
#include "pico/multicore.h"
#include "hardware/dma.h"
#include "Serial/config.h"
#include "Matrix/matrix.h"
#include "Serial/serial_uart/serial_uart.h"

static packet buffers[2];
static volatile uint8_t buffer = 0;
static int serial_dma_chan;

void __not_in_flash_func(serial_task)() {  
    serial_uart_task();
}

void __not_in_flash_func(serial_uart_callback)(uint8_t **buf, uint16_t *len) {
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
    serial_uart_start(serial_dma_chan); 
}

