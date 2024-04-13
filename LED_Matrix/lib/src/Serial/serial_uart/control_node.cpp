/* 
 * File:   control_node.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "hardware/uart.h"
#include "pico/multicore.h"
#include "Serial/serial_uart/control_node.h"
#include "Serial/serial_uart/data_node.h"
#include "Serial/serial_uart/CRC.h"

namespace Serial::UART::CONTROL_NODE {
    static uint8_t id = 0;

    static bool get_message(Control_Message *msg, uint32_t *checksum);
    
    void __not_in_flash_func(control_node)() {
        static Control_Message message;
        static uint32_t checksum = 0xFFFFFFFF;

        // Never respond to control messages
        if (get_message(&message, &checksum)) {
            if (message.checksum == ~checksum) {
                switch (message.payload) {
                    case 0:
                        if (message.id == 0 || message.id == id)
                            Serial::UART::DATA_NODE::trigger_processing();
                        break;

                    case 1:
                        if (message.id == 0 || message.id == id)
                            Serial::UART::DATA_NODE::reset();
                        break;

                    default:
                        break;
                }
            }

            checksum = 0xFFFFFFFF;
        }
    }

    void __not_in_flash_func(set_id)(uint8_t num) {
        id = num;
    }

    // Use uart1
    bool __not_in_flash_func(get_message)(Control_Message *msg, uint32_t *checksum) {
        static uint32_t index = 0;
        uint8_t c;

        if (uart_is_readable(uart0)) {
            c = uart_getc(uart0);
            // TODO: Store it
            *checksum = Serial::UART::CRC::crc32(*checksum, c);
            index++;
        }

        if (index == sizeof(Control_Message)) {
            index = 0;
            return true;
        }

        return false;
    }
}