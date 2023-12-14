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
    static packet buffers[2];
    static volatile uint8_t buffer = 0;

    void __not_in_flash_func(serial_task)() {  
        serial_uart_task();
    }

    void __not_in_flash_func(serial_uart_callback)(uint8_t **buf, uint16_t *len) {
        *buf = (uint8_t *) &buffers[(buffer + 1) % 2];
        *len = sizeof(packet);
        buffer = (buffer + 1) % 2;
    }

    void serial_start() {
        multicore_launch_core1(Matrix::work);
        
        int serial_dma_chan0 = dma_claim_unused_channel(true);
        int serial_dma_chan1 = dma_claim_unused_channel(true);
        serial_uart_start(serial_dma_chan0, serial_dma_chan1); 
    }
}
