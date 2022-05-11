/* 
 * File:   serial_spi.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef SERIAL_SPI_H
#define SERIAL_SPI_H

    typedef void (*serial_spi_callback)(uint8_t **buf, uint16_t *len);
    
    void serial_spi_start(serial_spi_callback callback, int dma_chan);
    void serial_spi_isr();
    
#endif
