/* 
 * File:   matrix.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#include "pico/multicore.h"
#include "Matrix/matrix.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"

namespace Matrix {
    volatile int timer;

    void start() {
        // Do nothing

        const int TX = 8;
        const int RX = 9;

        gpio_init(TX);
        gpio_init(RX);
        gpio_set_dir(TX, GPIO_OUT);
        gpio_set_function(TX, GPIO_FUNC_UART);
        gpio_set_function(RX, GPIO_FUNC_UART);
        uart_init(uart1, 115200);

        timer = hardware_alarm_claim_unused(true);
    }

    void __not_in_flash_func(dma_isr)() {
        // Do nothing
    }

    void __not_in_flash_func(timer_isr)() {
        // Do nothing
    }
}
