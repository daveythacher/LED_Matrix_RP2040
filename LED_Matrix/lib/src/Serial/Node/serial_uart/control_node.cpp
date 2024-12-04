/* 
 * File:   control_node.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include <stdint.h>
#include "hardware/gpio.h"
#include "hardware/uart.h"
#include "Serial/Node/control.h"
#include "Serial/Node/serial_uart/serial_uart.h"
#include "GPIO/GPIO.h"

namespace Serial::Node::Control {
    void start() {
        // IO
        gpio_init(5);
        gpio_set_function(5, GPIO_FUNC_UART);
        IO::claim(5);

        // UART
        static_assert(Serial::UART::SERIAL_UART_BAUD <= 7800000, "Baud rate must be less than 7.8MBaud");

        // With CPU the required tick rate could be as low as 20.5uS (We have no framing, which means no packets. DMA could soften this if we did.)
        //  This is higher priority than uart0.
        //      However the host protocol design should block this from ever becoming an issue.
        //  We only receive on this port. (RX is critical.)
        //      RX is not protected by protocol. (Failure can be observed by host.)
        //          Host has ability to recover bus by daemon (bootloader) and/or by control procedure.
        //              Watchdog and timeout will yield controller back to daemon and/or control procedure.
        uart_init(uart1, Serial::UART::SERIAL_UART_BAUD);

        // Future: Enable Hardware Flow control
    }

    // Warning host is required to obey flow control and handle bus recovery
    void __not_in_flash_func(task)() {
        // Check for errors
        if (!((uart1_hw->ris & 0x380) == 0)) {
            uart1_hw->icr = 0x7FF;
        }
    }

    bool __not_in_flash_func(isAvailable)() {
        return uart_is_readable(uart1);
    }

    uint8_t __not_in_flash_func(getc)() {
        return uart_getc(uart1);
    }
}
