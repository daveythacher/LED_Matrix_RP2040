/* 
 * File:   UART.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include <stdint.h>
#include "hardware/gpio.h"
#include "hardware/uart.h"
#include "hardware/resets.h"
#include "Interface/Node/UART/UART.h"
#include "Interface/Node/UART/hw_config.h"
#include "GPIO/GPIO.h"

namespace Interface::Node::UART {
    UART::UART() {
        // IO
        gpio_init(0);
        gpio_init(1);
        gpio_init(2);
        gpio_init(3);
        gpio_set_dir(0, GPIO_OUT);
        gpio_set_dir(3, GPIO_OUT);
        gpio_set_function(0, GPIO_FUNC_UART);
        gpio_set_function(1, GPIO_FUNC_UART);
        gpio_set_function(2, GPIO_FUNC_UART);
        gpio_set_function(3, GPIO_FUNC_UART);
        IO::GPIO::claim(0);
        IO::GPIO::claim(1);
        IO::GPIO::claim(2);
        IO::GPIO::claim(3);

        // UART
        reset_block(uart_get_index(uart0) ? RESETS_RESET_UART1_BITS : RESETS_RESET_UART0_BITS);
        unreset_block_wait(uart_get_index(uart0) ? RESETS_RESET_UART1_BITS : RESETS_RESET_UART0_BITS);
        uart_set_baudrate(uart0, SERIAL_UART_BAUD);
        uart_set_format(uart0, 8, 1, UART_PARITY_NONE);
        hw_set_bits(&uart_get_hw(uart0)->lcr_h, UART_UARTLCR_H_FEN_BITS);
        uart_get_hw(uart0)->dmacr = UART_UARTDMACR_TXDMAE_BITS | UART_UARTDMACR_RXDMAE_BITS;
        uart_get_hw(uart0)->cr = UART_UARTCR_UARTEN_BITS | UART_UARTCR_TXE_BITS | UART_UARTCR_RXE_BITS | UART_UARTCR_RTSEN_BITS | UART_UARTCR_CTSEN_BITS;
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
