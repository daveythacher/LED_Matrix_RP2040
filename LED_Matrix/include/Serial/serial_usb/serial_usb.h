/* 
 * File:   serial_usb.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef SERIAL_USB_H
#define SERIAL_USB_H

    #include <stdint.h>
    #include <stdio.h>

    typedef void (*serial_usb_callback)(uint8_t **buf, uint16_t *len, uint8_t num);
    
    void serial_usb_start(serial_usb_callback callback, int dma_chan0, int dma_chan1);
    void serial_usb_isr();
    uint32_t serial_get_chan_count();
    
#endif
