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
    static void __not_in_flash_func(task_internal)() {
        uart_task();
    }

    void __not_in_flash_func(task)() {  
        if (isPacket)
            task_internal();
    }

    void __not_in_flash_func(uart_callback)(uint8_t **buf, uint16_t *len) {
        if (isPacket) {
            static packet buffers[num_packets];
            static volatile uint8_t buffer = 0;

            *buf = (uint8_t *) &buffers[(buffer + 1) % num_packets];
            *len = sizeof(packet);
            buffer = (buffer + 1) % num_packets;

            static_assert(sizeof(packet) % 2 == 0, "Serial frame must be multiple of two");
        }
        else {
            *buf = (uint8_t *) Matrix::Loafer::get_back_buffer();
            *len = Matrix::Loafer::get_buffer_size();
        }
    }

    void start() {
        if (isPacket)
            multicore_launch_core1(Matrix::Worker::work);
        else
            multicore_launch_core1(task_internal);
        
        uart_start(dma_claim_unused_channel(true), dma_claim_unused_channel(true)); 
    }
}
