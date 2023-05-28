/* 
 * File:   serial_uart.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef SERIAL_UART_H
#define SERIAL_UART_H

    #include <stdint.h>
    #include <stdio.h>

    constexpr unsigned int SERIAL_UART_BAUD = DEFINE_SERIAL_UART_BAUD;

    typedef void (*serial_uart_callback)(uint8_t **buf, uint16_t *len);
    
    void serial_uart_start(serial_uart_callback callback, int dma_chan);
    void serial_uart_isr();
    void serial_uart_task();
    
#endif
