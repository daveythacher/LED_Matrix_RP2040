/* 
 * File:   serial_uart.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef SERIAL_UART_H
#define SERIAL_UART_H

    #include <stdint.h>
    #include <stdio.h>

    typedef void (*serial_pmp_callback)(uint8_t **buf, uint16_t *len);
    
    void serial_pmp_start(serial_pmp_callback callback, int dma_chan);
    void serial_pmp_isr();
    
#endif
