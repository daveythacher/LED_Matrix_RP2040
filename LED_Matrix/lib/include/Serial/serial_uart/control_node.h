/* 
 * File:   control_node.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef SERIAL_UART_CONTROL_NODE_H
#define SERIAL_UART_CONTROL_NODE_H
    
#include "Serial/serial_uart/types.h"

namespace Serial::UART::CONTROL_NODE {
    void control_node();
    void set_id(uint8_t id);
}

#endif
