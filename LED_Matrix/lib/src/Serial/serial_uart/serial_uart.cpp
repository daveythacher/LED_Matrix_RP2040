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

        // With CPU the required tick rate could be as low as 20.5uS (We have no framing, which means no packets. DMA could soften this if we did.)
        //  We send and receive on this port. (RX is critical, TX is protected by protocol.)
        //      RX should be protected by TX.
        uart_init(uart0, SERIAL_UART_BAUD);

        // With CPU the required tick rate could be as low as 20.5uS (We have no framing, which means no packets. DMA could soften this if we did.)
        //  This is higher priority than uart0.
        //      However the host protocol design should block this from ever becoming an issue.
        //  We only receive on this port. (RX is critical.)
        //      RX is not protected by protocol. (Failure can be observed by host.)
        //          Host has ability to recover bus by daemon (bootloader) and/or by control procedure.
        //              Watchdog and timeout will yield controller back to daemon and/or control procedure.
        uart_init(uart1, SERIAL_UART_BAUD);

        // Setup TCAM rules
        Serial::UART::DATA_NODE::data_node_setup();

        // Future: Enable Hardware Flow control
    }

    // Warning host is required to obey flow control and handle bus recovery
    void __not_in_flash_func(uart_task)() {
        static uint64_t time = time_us_64();
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

        // Do not block, let flow control do it's thing
        if ((time_us_64() - time) >= (((10 * sizeof(Serial::UART::internal::Status_Message) * 1000000) / SERIAL_UART_BAUD) + 1)) {
            Serial::UART::internal::send_status(status);
            time = time_us_64();
        }
    }
    
    void __not_in_flash_func(uart_callback)(Serial::packet **buf) {
        static Serial::packet buffers[num_packets];
        static volatile uint8_t buffer = 0;

        *buf = &buffers[(buffer + 1) % num_packets];
        buffer = (buffer + 1) % num_packets;
    }    

    uint16_t __not_in_flash_func(uart_get_len)() {
        return sizeof(Serial::packet);
    }
}
