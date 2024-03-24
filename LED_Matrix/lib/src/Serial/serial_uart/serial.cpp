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
        do {
            *buf = (uint8_t *) Matrix::Loafer::get_back_buffer();
            *len = Matrix::Loafer::get_buffer_size();
        } while (*buf != nullptr);
    }

    void start() {        
        uart_start(dma_claim_unused_channel(true), dma_claim_unused_channel(true)); 
    }
}
