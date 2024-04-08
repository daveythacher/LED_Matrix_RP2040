/* 
 * File:   internal.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef SERIAL_UART_INTERNAL_H
#define SERIAL_UART_INTERNAL_H
    
#include "Serial/serial_uart/types.h"

namespace Serial::internal {
    void uart_process(uint16_t *buf, uint16_t len);

    STATUS data_node();
    void control_node();
    void set_id(uint8_t id);

    void send_status(STATUS status);
    void send_message(Status_Message *message, bool block);
}

#endif
