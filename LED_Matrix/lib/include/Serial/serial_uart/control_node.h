/* 
 * File:   control_node.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef SERIAL_UART_CONTROL_NODE_H
#define SERIAL_UART_CONTROL_NODE_H
    
#include <stdint.h>

namespace Serial::UART::CONTROL_NODE {
    struct Control_Message {
        uint32_t header;
        uint8_t cmd;
        uint16_t len;
        uint8_t id;
        uint32_t checksum;
        uint32_t delimiter;
    };

    void control_node();
    void set_id(uint8_t id);
}

#endif
