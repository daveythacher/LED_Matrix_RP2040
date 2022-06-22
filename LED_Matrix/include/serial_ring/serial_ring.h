/* 
 * File:   serial_ring.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef SERIAL_RING_H
#define SERIAL_RING_H

    typedef void (*serial_ring_callback)(uint8_t **buf_rx, uint8_t **buf_tx, uint16_t *len);
    
    void serial_ring_start(serial_ring_callback callback, int dma_chan_rx, int dma_chan_tx);
    void serial_ring_isr();
    
#endif
