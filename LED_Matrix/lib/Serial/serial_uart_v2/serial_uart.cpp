/* 
 * File:   serial_uart.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#include <stdint.h>
#include "hardware/gpio.h"
#include "hardware/dma.h"
#include "hardware/uart.h"
#include "hardware/timer.h"
#include "Serial/serial_uart_v2/serial_uart.h"
#include "Matrix/matrix.h"

static serial_uart_callback func;
static int dma_chan;
static dma_channel_config c;

static void serial_uart_reload(bool isNew);

void serial_uart_start(serial_uart_callback callback, int dma) {
    func = callback;
    dma_chan = dma;
    
    // IO
    gpio_init(0);
    gpio_init(1);
    gpio_init(2);
    gpio_init(3);
    gpio_set_dir(0, GPIO_OUT);
    gpio_set_dir(3, GPIO_OUT);
    gpio_set_function(0, GPIO_FUNC_UART);
    gpio_set_function(1, GPIO_FUNC_UART);
    gpio_set_mask(1 << 3);
    gpio_set_irq_enabled(2, GPIO_IRQ_EDGE_RISE, true);
    
    // UART
    uart_init(uart0, 4000000);
    
    // DMA
    c = dma_channel_get_default_config(dma_chan);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
    channel_config_set_write_increment(&c, true);
    channel_config_set_read_increment(&c, false);
    channel_config_set_dreq(&c, DREQ_UART0_RX);
    
    serial_uart_reload(true);
}

// Note: Blocking call
void serial_uart_print(const char *s) {
    gpio_clr_mask(1 << 3);
    uart_puts(uart0, s);
    // Wait for transmitter to idle with FIFO empty
    while (!(uart0_hw->fr & 1 << 7) || (uart0_hw->fr & 1 << 3));
    gpio_set_mask(1 << 3);
    // Hold transmitter idle for 1uS (This allows interrupt to fire.)
    uint64_t time = time_us_64();
    while((time_us_64() - time) < 2);
}

void __not_in_flash_func(serial_uart_reload)(bool isNew) {
    static uint8_t *ptr = 0;
    static uint8_t *buf;
    static uint16_t len;
        
    if (isNew)
        func(&buf, &len);
    dma_channel_configure(dma_chan, &c, buf, &uart_get_hw(uart0)->dr, len, true);
    
    if (ptr)
        process((void *) ptr);
    ptr = buf;
}

void __not_in_flash_func(serial_uart_isr)() {
    if (gpio_get_irq_event_mask(2) & GPIO_IRQ_EDGE_RISE) {
        if (dma_hw->intr & (1 << dma_chan))
            serial_uart_reload(true);
        else {
            dma_channel_abort(dma_chan);
            serial_uart_reload(false);
        }
        gpio_acknowledge_irq(2, GPIO_IRQ_EDGE_RISE);
    }
}

