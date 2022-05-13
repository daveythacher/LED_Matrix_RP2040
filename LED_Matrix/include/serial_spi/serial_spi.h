/* 
 * File:   serial_spi.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef SERIAL_SPI_H
#define SERIAL_SPI_H

    #include "hardware/pio.h"

    typedef void (*serial_spi_callback)(uint8_t **buf, uint16_t *len);
    
    void serial_spi_start(serial_spi_callback callback, int dma_chan, PIO p, int pio_sm);
    void serial_spi_isr();
    
#endif
