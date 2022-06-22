/* 
 * File:   serial_ring.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#include <stdint.h>
#include "hardware/gpio.h"
#include "hardware/dma.h"
#include "hardware/uart.h"
#include "serial_ring/serial_ring.h"

static serial_ring_callback func;
static int dma_chan_rx;
static int dma_chan_tx;
static dma_channel_config c_rx;
static dma_channel_config c_tx;

static void serial_ring_reload();

void serial_ring_start(serial_ring_callback callback, int dma_rx, int dma_tx) {
    func = callback;
    dma_chan_rx = dma_rx;
    dma_chan_tx = dma_tx;
    
    // IO
    gpio_init(0);
    gpio_init(1);
    gpio_set_dir(0, GPIO_OUT);
    gpio_set_function(0, GPIO_FUNC_UART);
    gpio_set_function(1, GPIO_FUNC_UART);
    
    // UART
    uart_init(uart0, 7800000);
    
    // DMA
    c_rx = dma_channel_get_default_config(dma_chan_rx);
    channel_config_set_transfer_data_size(&c_rx, DMA_SIZE_8);
    channel_config_set_write_increment(&c_rx, true);
    channel_config_set_read_increment(&c_rx, false);
    channel_config_set_dreq(&c_rx, DREQ_UART0_RX);
    dma_channel_set_irq1_enabled(dma_chan_rx, true);
    
    c_tx = dma_channel_get_default_config(dma_chan_tx);
    channel_config_set_transfer_data_size(&c_tx, DMA_SIZE_8);
    channel_config_set_write_increment(&c_tx, false);
    channel_config_set_read_increment(&c_tx, true);
    channel_config_set_dreq(&c_tx, DREQ_UART0_TX);
    
    serial_ring_reload();
}

// Warning: Weak protocol/state machine!
void __not_in_flash_func(serial_ring_reload)() {
    uint8_t *buf_rx;
    uint8_t *buf_tx;
    uint16_t len;
        
    func(&buf_rx, &buf_tx, &len);
    dma_channel_configure(dma_chan_tx, &c_tx, &uart_get_hw(uart0)->dr, buf_tx, len, true);
    dma_channel_configure(dma_chan_rx, &c_rx, buf_rx, &uart_get_hw(uart0)->dr, len, true);
}

void __not_in_flash_func(serial_ring_isr)() {
    if (dma_channel_get_irq1_status(dma_chan_rx)) {
        serial_ring_reload();
        dma_hw->ints0 = 1 << dma_chan_rx;
    }
}

