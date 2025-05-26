/* 
 * File:   data_node.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include <stdint.h>
#include "hardware/gpio.h"
#include "hardware/uart.h"
#include "Serial/Node/Node.h"
#include "Serial/Node/serial_uart/serial_uart.h"
#include "GPIO/GPIO.h"

namespace Serial::Node::Data {
    void init() {
        // IO
        gpio_init(0);
        gpio_init(1);
        gpio_set_dir(0, GPIO_OUT);
        gpio_set_function(0, GPIO_FUNC_UART);
        gpio_set_function(1, GPIO_FUNC_UART);
        IO::GPIO::claim(0);
        IO::GPIO::claim(1);

        // UART
        uart_init(uart0, Serial::UART::SERIAL_UART_BAUD);

        // TODO: Enable Hardware Flow control
    }

    bool __not_in_flash_func(isAvailable)() {
        // Check for errors
        if (!((uart0_hw->ris & 0x780) == 0)) {
            uart0_hw->rsr = 0xF;
            uart0_hw->icr = 0x7FF;
        }

        return uart_is_readable(uart0);
    }

    uint8_t __not_in_flash_func(getc)() {
        return uart_getc(uart0);
    }

    void __not_in_flash_func(putc)(uint8_t c) {
        uart_putc(uart0, c);
    }
}
