/* 
 * File:   serial_uart.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef SERIAL_UART_H
#define SERIAL_UART_H

    #include <stdint.h>
    #include <stdio.h>
    
    void serial_uart_start(int dma_chan);
    void serial_uart_isr();
    void serial_uart_task();
    void serial_uart_callback(uint8_t **buf, uint16_t *len);


    // -- DO NOT EDIT BELOW THIS LINE --

    #include "../config.h"

    constexpr unsigned int SERIAL_UART_BAUD = DEFINE_SERIAL_UART_BAUD;
    
#endif
