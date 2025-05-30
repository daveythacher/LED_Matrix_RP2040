/* 
 * File:   UART.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include <stdint.h>
#include "hardware/gpio.h"
#include "hardware/uart.h"
#include "Interface/Node/UART/UART.h"
#include "Interface/Node/UART/hw_config.h"
#include "GPIO/GPIO.h"

namespace Interface::Node::UART {
    UART::UART() {
        // IO
        gpio_init(0);
        gpio_init(1);
        gpio_set_dir(0, GPIO_OUT);
        gpio_set_function(0, GPIO_FUNC_UART);
        gpio_set_function(1, GPIO_FUNC_UART);
        IO::GPIO::claim(0);
        IO::GPIO::claim(1);

        // UART
        uart_init(uart0, SERIAL_UART_BAUD);

        // TODO: Enable Hardware Flow control
    }

    UART *UART::create_node() {
        return new UART();
    }

    bool UART::get_available() {
        // Check for errors
        if (!((uart0_hw->ris & 0x780) == 0)) {
            uart0_hw->rsr = 0xF;
            uart0_hw->icr = 0x7FF;
        }

        return uart_is_readable(uart0);
    }

    bool UART::put_available() {
        // Check for errors
        if (!((uart0_hw->ris & 0x780) == 0)) {
            uart0_hw->rsr = 0xF;
            uart0_hw->icr = 0x7FF;
        }

        return uart_is_writable(uart0);
    }

    uint8_t UART::get() {
        return uart_getc(uart0);
    }

    void UART::put(uint8_t c) {
        uart_putc(uart0, c);
    }
}
