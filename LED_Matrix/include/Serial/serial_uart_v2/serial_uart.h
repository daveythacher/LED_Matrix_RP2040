/* 
 * File:   serial_uart.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef SERIAL_UART_H
#define SERIAL_UART_H

    #include <stdint.h>
    #include <stdio.h>

    typedef void (*serial_uart_callback)(uint8_t **buf, uint16_t *len);
    
    void serial_uart_start(serial_uart_callback callback, int dma_chan);
    void serial_uart_isr();
    void serial_uart_print(const char *s);
    
    #define serial_uart_printf(format, ...) { static char str[100]; snprintf(str, sizeof(str), format, ##__VA_ARGS__); serial_uart_print(str); }
    
#endif
