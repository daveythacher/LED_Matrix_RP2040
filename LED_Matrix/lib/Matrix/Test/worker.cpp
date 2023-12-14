/* 
 * File:   worker.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#include <stdint.h>
#include "pico/multicore.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"
#include "Serial/config.h"
#include "Matrix/matrix.h"
#include "Matrix/helper.h"

namespace Matrix::Worker {
    void __not_in_flash_func(work)() {
        const int TX = 8;
        const int RX = 9;

        gpio_init(TX);
        gpio_init(RX);
        gpio_set_dir(TX, GPIO_OUT);
        gpio_set_function(TX, GPIO_FUNC_UART);
        gpio_set_function(RX, GPIO_FUNC_UART);
        uart_init(uart1, 115200);
        
        while(1) {
            Serial::packet *p = (Serial::packet *) APP::multicore_fifo_pop_blocking_inline();

            uart_write_blocking(uart1, (uint8_t *) p, sizeof(Serial::packet));
        }
    }

    bool __not_in_flash_func(process)(void *arg, bool block) {
        if (multicore_fifo_wready() || block) {
            APP::multicore_fifo_push_blocking_inline((uint32_t) arg);
            return true;
        }

        return false;
    }
}
