/* 
 * File:   loafer.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#include <stdint.h>
#include "pico/multicore.h"
#include "hardware/uart.h"
#include "Serial/config.h"
#include "Matrix/matrix.h"

namespace Matrix::Loafer {
    void __not_in_flash_func(toss)(void *arg) {
        uart_write_blocking(uart1, (uint8_t *) arg, Serial::payload_size);
    }

    void *__not_in_flash_func(get_back_buffer)() {
        static volatile uint8_t back_buffer[Serial::payload_size];

        return (void *) back_buffer;
    }

    uint32_t __not_in_flash_func(get_buffer_size)() {
        static_assert(Serial::payload_size % 2 == 0, "Loafer buffer must be multiple of two");
        return Serial::payload_size;
    }
}
