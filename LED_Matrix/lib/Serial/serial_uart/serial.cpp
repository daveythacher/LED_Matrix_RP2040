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
    static packet buffers[num_packets];
    static volatile uint8_t buffer = 0;

    void __not_in_flash_func(task)() {  
        uart_task();
    }

    void __not_in_flash_func(uart_callback)(uint8_t **buf, uint16_t *len) {
        if (isPacket) {
            *buf = (uint8_t *) &buffers[(buffer + 1) % num_packets];
            *len = sizeof(packet);
            buffer = (buffer + 1) % num_packets;
        }
        else {
            *buf = (uint8_t *) Matrix::Loafer::get_back_buffer();
            *len = payload_size;
        }
    }

    void start() {
        multicore_launch_core1(Matrix::Worker::work);
        
        int serial_dma_chan0 = dma_claim_unused_channel(true);
        int serial_dma_chan1 = dma_claim_unused_channel(true);
        uart_start(serial_dma_chan0, serial_dma_chan1); 
    }
}
