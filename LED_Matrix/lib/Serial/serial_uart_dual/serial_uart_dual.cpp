/*
 * File:   serial_uart_dual.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include <stdint.h>
#include <machine/endian.h>
#include "hardware/gpio.h"
#include "hardware/dma.h"
#include "hardware/uart.h"
#include "hardware/timer.h"
#include "Serial/serial_uart_dual/serial_uart_dual.h"
#include "Matrix/matrix.h"

static serial_uart_dual_callback func;
static int dma_chan[2];
static dma_channel_config c;
static volatile bool isIdle = true;
static volatile bool isClean = true;

static void serial_uart_dual_reload(bool reload_dma, bool process_msg);

#if __BYTE_ORDER == __LITTLE_ENDIAN
    #define ntohs(x) __bswap16(x)
#else
    #define ntohs(x) ((uint16_t)(x))
#endif

void serial_uart_dual_start(serial_uart_dual_callback callback, int dma[2]) {
    func = callback;
    dma_chan[0] = dma[0];
    dma_chan[1] = dma[1];

    // IO
    gpio_init(0);
    gpio_init(1);
    gpio_init(4);
    gpio_init(5);
    gpio_set_dir(0, GPIO_OUT);
    gpio_set_dir(4, GPIO_OUT);
    gpio_set_function(0, GPIO_FUNC_UART);
    gpio_set_function(1, GPIO_FUNC_UART);
    gpio_set_function(4, GPIO_FUNC_UART);
    gpio_set_function(5, GPIO_FUNC_UART);

    // UART
    uart_init(uart0, 7800000);
    uart_init(uart1, 7800000);

    // DMA
    c = dma_channel_get_default_config(dma_chan[0]);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
    channel_config_set_write_increment(&c, true);
    channel_config_set_read_increment(&c, false);
    channel_config_set_dreq(&c, DREQ_UART0_RX);
    dma_channel_set_irq1_enabled(dma_chan[0], true);
    c = dma_channel_get_default_config(dma_chan[1]);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
    channel_config_set_write_increment(&c, true);
    channel_config_set_read_increment(&c, false);
    channel_config_set_dreq(&c, DREQ_UART1_RX);
    dma_channel_set_irq1_enabled(dma_chan[1], true);

    serial_uart_dual_reload(false, true);
}

void __not_in_flash_func(serial_uart_dual_task)() {
    static uint64_t time = time_us_64(); 

    if (isIdle) {
        // Clear Errors
        uart0_hw->icr = 0x7FF;
        uart1_hw->icr = 0x7FF;
        isClean = true;

        // Send idle token
        if ((time + 10) < time_us_64()) {
            uart_putc(uart0, 'i');
            time = time_us_64();
        }

        // Look for start_token
        if (uart_getc(uart0) == 's') {
            serial_uart_dual_reload(true, false);
            isIdle = false;
        }

    }
}

void __not_in_flash_func(serial_uart_dual_reload)(bool reload_dma, bool process_msg) {
    static uint8_t *ptr = 0;
    static uint8_t *buf = 0;
    static uint16_t len = 0;
    uint16_t *p = (uint16_t *)buf;
    
    if (reload_dma) {
        dma_channel_configure(dma_chan[0], &c, buf, &uart_get_hw(uart0)->dr, len / 2, true);
        dma_channel_configure(dma_chan[1], &c, buf + (len / 2), &uart_get_hw(uart1)->dr, len / 2, true);
    }
    else if (process_msg) {
        if (p != 0) {
            for (uint16_t i = 0; i < len; i += 2)
                p[i / 2] = ntohs(p[i / 2]);
        }

        func(&buf, &len);

        if (ptr)
            process((void *)ptr);
            
        ptr = buf;
    }
}

void __not_in_flash_func(serial_uart_dual_isr)() {
    static uint8_t count = 0;

    if (dma_channel_get_irq1_status(dma_chan[0])) {
        if (uart0_hw->ris & 0x380)
            isClean = false;

        if (++count >= 2) {
            serial_uart_dual_reload(false, isClean);
            isIdle = true;
            count = 0;
        }

        dma_hw->ints1 = 1 << dma_chan[0];
    }

    if (dma_channel_get_irq1_status(dma_chan[1])) {
        if (uart1_hw->ris & 0x380)
            isClean = false;

        if (++count >= 2) {
            serial_uart_dual_reload(false, isClean);
            isIdle = true;
            count = 0;
        }

        dma_hw->ints1 = 1 << dma_chan[1];
    }
}
