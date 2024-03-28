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

namespace Serial {
    void __not_in_flash_func(task)() { 
        uart_task();
    }

    void __not_in_flash_func(uart_callback)(uint8_t **buf, uint16_t *len) {
        static packet buffers[num_packets];
        static volatile uint8_t buffer = 0;

        *buf = (uint8_t *) &buffers[(buffer + 1) % num_packets];
        *len = sizeof(packet);
        buffer = (buffer + 1) % num_packets;

        static_assert(sizeof(packet) % 2 == 0, "Serial frame must be multiple of two");
    }

    void start() {        
        uart_start(dma_claim_unused_channel(true), dma_claim_unused_channel(true)); 
    }
}
