/* 
 * File:   serial_uart.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#include <stdint.h>
#include "hardware/gpio.h"
#include "hardware/dma.h"
#include "hardware/pio.h"
#include "Serial/serial_pmp/serial_pmp.h"
#include "Matrix/matrix.h"

static serial_pmp_callback func;
static int dma_chan;
static dma_channel_config c;

static void serial_pmp_reload();

void serial_pmp_start(serial_pmp_callback callback, int dma) {
    func = callback;
    dma_chan = dma;
    
    // IO
    // TODO:
    
    // PMP
    // TODO:
    
    // DMA
    c = dma_channel_get_default_config(dma_chan);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
    channel_config_set_write_increment(&c, true);
    channel_config_set_read_increment(&c, false);
    channel_config_set_dreq(&c, DREQ_UART0_RX);
    dma_channel_set_irq1_enabled(dma_chan, true);
    
    serial_pmp_reload();
}

void __not_in_flash_func(serial_pmp_reload)() {
    static uint8_t *ptr = 0;
    static uint8_t *buf;
    static uint16_t len;
        
    func(&buf, &len);
    // TODO: dma_channel_configure(dma_chan, &c, buf, &uart_get_hw(uart0)->dr, len, true);
    
    if (ptr)
        process((void *) ptr);
    ptr = buf;
}

void __not_in_flash_func(serial_pmp_isr)() {
    if (dma_channel_get_irq1_status(dma_chan)) {
        serial_pmp_reload();
        dma_hw->ints1 = 1 << dma_chan;
    }
}

