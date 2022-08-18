/* 
 * File:   serial_uart.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#include <stdint.h>
#include "hardware/gpio.h"
#include "hardware/dma.h"
#include "hardware/uart.h"
#include "Serial/serial_uart/serial_uart.h"

static serial_uart_callback func;
static int dma_chan;
static dma_channel_config c;

static void serial_uart_reload();

void serial_uart_start(serial_uart_callback callback, int dma) {
    func = callback;
    dma_chan = dma;
    
    // IO
    gpio_init(0);
    gpio_init(1);
    gpio_set_dir(0, GPIO_OUT);
    gpio_set_function(0, GPIO_FUNC_UART);
    gpio_set_function(1, GPIO_FUNC_UART);
    
    // UART
    uart_init(uart0, 4000000);
    
    // DMA
    c = dma_channel_get_default_config(dma_chan);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
    channel_config_set_write_increment(&c, true);
    channel_config_set_read_increment(&c, false);
    channel_config_set_dreq(&c, DREQ_UART0_RX);
    dma_channel_set_irq1_enabled(dma_chan, true);
    
    serial_uart_reload();
}

void serial_uart_print(const char *s) {
    uart_puts(uart0, s);
}

void __not_in_flash_func(serial_uart_reload)() {
    uint8_t *buf;
    uint16_t len;
        
    func(&buf, &len);
    dma_channel_configure(dma_chan, &c, buf, &uart_get_hw(uart0)->dr, len, true);
}

void __not_in_flash_func(serial_uart_isr)() {
    if (dma_channel_get_irq1_status(dma_chan)) {
        serial_uart_reload();
        dma_hw->ints1 = 1 << dma_chan;
    }
}

