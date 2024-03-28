/*
 * File:   serial_uart.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include <stdint.h>
#include <machine/endian.h>
#include "hardware/gpio.h"
#include "hardware/uart.h"
#include "Serial/serial_uart/serial_uart.h"
#include "Matrix/matrix.h"

namespace Serial {
    static uint8_t *buf = 0;
    static uint16_t len = 0;
    static uint8_t state = 0;

    static void uart_process();

    #if __BYTE_ORDER == __LITTLE_ENDIAN
        #define ntohs(x) __bswap16(x)
        #define ntohl(x) __bswap32(x)
    #else
        #define ntohs(x) ((uint16_t)(x))
        #define ntoh1(x) ((uint32_t)(x))
    #endif

    void uart_start() {
        // IO
        gpio_init(0);
        gpio_init(1);
        gpio_set_dir(0, GPIO_OUT);
        gpio_set_function(0, GPIO_FUNC_UART);
        gpio_set_function(1, GPIO_FUNC_UART);

        // UART
        static_assert(SERIAL_UART_BAUD <= 7800000, "Baud rate must be less than 7.8MBaud");
        uart_init(uart0, SERIAL_UART_BAUD);
    }

    // Warning host is required to obey flow control and handle bus recovery
    void __not_in_flash_func(uart_task)() {
        // Check for errors
        if (!((uart0_hw->ris & 0x380) == 0)) {
            uart0_hw->icr = 0x7FF;
        }

        // TODO:
        switch (state) {
            case 0:
                uart_callback(&buf, &len);
                break;
            case 10:
                uart_process();
                break;
            default:
                break;
        }
    }

    void __not_in_flash_func(uart_process)() {
        uint16_t *p = (uint16_t *) buf;

        switch (sizeof(DEFINE_SERIAL_RGB_TYPE)) {
            case 2:
            case 6:
                for (uint16_t i = 0; i < len; i += 2)
                    p[i / 2] = ntohs(p[i / 2]);
                break;
            default:
                break;
        }

        Matrix::Worker::process((void *) buf);
    }
}
