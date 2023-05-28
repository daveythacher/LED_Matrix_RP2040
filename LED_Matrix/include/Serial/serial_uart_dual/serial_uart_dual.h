/* 
 * File:   serial_uart_dual.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef SERIAL_UART_DUAL_H
#define SERIAL_UART_DUAL_H

    #include <stdint.h>
    #include <stdio.h>

    typedef void (*serial_uart_dual_callback)(uint8_t **buf, uint16_t *len);
    
    void serial_uart_dual_start(serial_uart_dual_callback callback, int dma_chan[2]);
    void serial_uart_dual_isr();
    void serial_uart_dual_task();
    
#endif
