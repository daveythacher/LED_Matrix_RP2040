/* 
 * File:   internal.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef SERIAL_UART_INTERNAL_H
#define SERIAL_UART_INTERNAL_H
    
#include "Serial/serial_uart/types.h"

namespace Serial::UART::internal {
    void process(uint16_t *buf, uint16_t len);
    void send_status(STATUS status);
    void send_message(Status_Message *message, bool block);
}

#endif
