/* 
 * File:   control_node.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "hardware/uart.h"
#include "pico/multicore.h"
#include "Serial/serial_uart/control_node.h"
#include "Serial/serial_uart/data_node.h"

namespace Serial::UART::CONTROL_NODE {
    static uint8_t id = 0;

    static bool get_message(Control_Message *msg);
    
    void __not_in_flash_func(control_node)() {
        static Control_Message message;

        // Never respond to control messages
        if (get_message(&message)) {
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
    }

    void __not_in_flash_func(set_id)(uint8_t num) {
        id = num;
    }

    // Use uart1
    // TODO: Switch to ISR?
    //  Use DMA?
    bool __not_in_flash_func(get_message)(Control_Message *msg) {
        // TODO: Nonblocking

        // TODO: Verify checksum
        return false;
    }
}