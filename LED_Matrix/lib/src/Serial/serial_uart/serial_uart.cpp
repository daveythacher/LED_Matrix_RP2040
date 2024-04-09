/*
 * File:   serial_uart.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include <stdint.h>
#include "hardware/gpio.h"
#include "hardware/uart.h"
#include "Serial/serial_uart/serial_uart.h"
#include "Serial/serial_uart/internal.h"
#include "Serial/serial_uart/control_node.h"
#include "Serial/serial_uart/data_node.h"
#include "Matrix/matrix.h"
using Serial::UART::internal::STATUS;

namespace Serial::UART {
    void uart_start() {
        // IO
        gpio_init(0);
        gpio_init(1);
        gpio_init(5);
        gpio_set_dir(0, GPIO_OUT);
        gpio_set_function(0, GPIO_FUNC_UART);
        gpio_set_function(1, GPIO_FUNC_UART);
        gpio_set_function(5, GPIO_FUNC_UART);

        // UART
        static_assert(SERIAL_UART_BAUD <= 7800000, "Baud rate must be less than 7.8MBaud");
        uart_init(uart0, SERIAL_UART_BAUD);
        uart_init(uart1, SERIAL_UART_BAUD);
    }

    // Warning host is required to obey flow control and handle bus recovery
    void __not_in_flash_func(uart_task)() {
        STATUS status;

        // Check for errors
        if (!((uart0_hw->ris & 0x380) == 0)) {
            uart0_hw->icr = 0x7FF;
        }

        // Check for errors
        if (!((uart1_hw->ris & 0x380) == 0)) {
            uart1_hw->icr = 0x7FF;
        }

        Serial::UART::CONTROL_NODE::control_node();
        status = Serial::UART::DATA_NODE::data_node();
        Serial::UART::internal::send_status(status);
    }
    
    void __not_in_flash_func(uart_callback)(uint8_t **buf, uint16_t *len) {
        static packet buffers[num_packets];
        static volatile uint8_t buffer = 0;

        *buf = (uint8_t *) &buffers[(buffer + 1) % num_packets];
        *len = sizeof(packet);
        buffer = (buffer + 1) % num_packets;
    }    
}
