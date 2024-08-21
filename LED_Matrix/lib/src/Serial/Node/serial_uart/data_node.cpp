/* 
 * File:   serial.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include <stdint.h>
#include "hardware/gpio.h"
#include "hardware/uart.h"
#include "Serial/Node/data.h"
#include "Serial/Node/serial_uart/serial_uart.h"

namespace Serial::Node::Data {
    void start() {
        // IO
        gpio_init(0);
        gpio_init(1);
        gpio_set_dir(0, GPIO_OUT);
        gpio_set_function(0, GPIO_FUNC_UART);
        gpio_set_function(1, GPIO_FUNC_UART);

        // UART
        static_assert(Serial::UART::SERIAL_UART_BAUD <= 7800000, "Baud rate must be less than 7.8MBaud");

        // With CPU the required tick rate could be as low as 20.5uS (We have no framing, which means no packets. DMA could soften this if we did.)
        //  We send and receive on this port. (RX is critical, TX is protected by protocol.)
        //      RX should be protected by TX.
        uart_init(uart0, Serial::UART::SERIAL_UART_BAUD);

        // Future: Enable Hardware Flow control
    }

    // Warning host is required to obey flow control and handle bus recovery
    void __not_in_flash_func(task)() {
        // Check for errors
        if (!((uart0_hw->ris & 0x380) == 0)) {
            uart0_hw->icr = 0x7FF;
        }
    }
    
    void __not_in_flash_func(callback)(Serial::packet **buf) {
        static Serial::packet buffers[num_packets];
        static volatile uint8_t buffer = 0;

        *buf = &buffers[(buffer + 1) % num_packets];
        buffer = (buffer + 1) % num_packets;
    }    

    uint16_t __not_in_flash_func(get_len)() {
        return sizeof(Serial::packet);
    }

    bool __not_in_flash_func(isAvailable)() {
        return uart_is_readable(uart0);
    }

    uint8_t __not_in_flash_func(getc)() {
        return uart_getc(uart0);
    }

    void __not_in_flash_func(putc)(uint8_t c) {
        uart_putc(uart0, c);
    }
}
