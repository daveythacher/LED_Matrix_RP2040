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
    bool __not_in_flash_func(toss)(void *arg, bool block) {
        uart_write_blocking(uart1, (uint8_t *) arg, Serial::payload_size);
        return false;
    }

    void *__not_in_flash_func(get_back_buffer)() {
        // TODO:

        return nullptr;
    }
}
