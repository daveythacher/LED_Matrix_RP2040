/*
 * File:   serial_uart.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include <stdint.h>
#include <machine/endian.h>
#include "hardware/gpio.h"
#include "hardware/dma.h"
#include "hardware/uart.h"
#include "hardware/timer.h"
#include "Serial/serial_uart/serial_uart.h"
#include "Matrix/matrix.h"

static int dma_chan;
static dma_channel_config c;
static volatile bool isIdle = true;
static volatile uint32_t checksum;

static void serial_uart_reload(bool reload_dma);

#if __BYTE_ORDER == __LITTLE_ENDIAN
    #define ntohs(x) __bswap16(x)
    #define ntohl(x) __bswap32(x)
#else
    #define ntohs(x) ((uint16_t)(x))
    #define ntoh1(x) ((uint32_t)(x))
#endif

void serial_uart_start(int dma) {
    dma_chan = dma;

    // IO
    gpio_init(0);
    gpio_init(1);
    gpio_set_dir(0, GPIO_OUT);
    gpio_set_function(0, GPIO_FUNC_UART);
    gpio_set_function(1, GPIO_FUNC_UART);

    // UART
    static_assert(SERIAL_UART_BAUD <= 7800000, "Baud rate must be less than 7.8MBaud");
    uart_init(uart0, SERIAL_UART_BAUD);

    // DMA
    c = dma_channel_get_default_config(dma_chan);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
    channel_config_set_write_increment(&c, true);
    channel_config_set_read_increment(&c, false);
    channel_config_set_dreq(&c, DREQ_UART0_RX);
    dma_channel_set_irq1_enabled(dma_chan, true);

    serial_uart_reload(false);
}

void __not_in_flash_func(serial_uart_task)() {
    static uint64_t time = time_us_64(); 
    static bool needsChecksum = false;
    static uint8_t counter = 0;

    if (isIdle) {
        // Clear Errors
        uart0_hw->icr = 0x7FF;

        // Send idle token
        if ((time + 10) < time_us_64()) {
            uart_putc(uart0, 'i');
            time = time_us_64();
        }

        // Look for start_token
        if (uart_is_readable(uart0) && uart_getc(uart0) == 's') {
            checksum = 0;
            counter = 0;
            needsChecksum = true;
            isIdle = false;
        }
    }

    // Wait for the checksum
    if (needsChecksum) {
        while (counter < 4 && uart_is_readable(uart0)) {
            checksum |= uart_getc(uart0) << (counter * 8);
            ++counter;
        }

        // Got it now start the transfer
        if (counter >= 4) {
            needsChecksum = false;
            checksum = ntohl(checksum);
            serial_uart_reload(true);
        }
    }
}

void __not_in_flash_func(serial_uart_reload)(bool reload_dma) {
    static uint8_t *ptr = 0;
    static uint8_t *buf = 0;
    static uint16_t len = 0;
    uint16_t *p = (uint16_t *) buf;
    
    // Note: Uses CRC-32/MPEG-2
    if (reload_dma) {
        dma_sniffer_set_data_accumulator(0xFFFFFFFF);
        dma_channel_configure(dma_chan, &c, buf, &uart_get_hw(uart0)->dr, len, true);
        dma_sniffer_enable(dma_chan, 0, true);
    }
    else {
        if ((p != 0) && (sizeof(DEFINE_SERIAL_RGB_TYPE) == sizeof(uint16_t))) {
            for (uint16_t i = 0; i < len; i += 2)
                p[i / 2] = ntohs(p[i / 2]);
        }

        serial_uart_callback(&buf, &len);

        if (ptr)
            process((void *) ptr);
    }
}

void __not_in_flash_func(serial_uart_isr)() {
    if (dma_channel_get_irq1_status(dma_chan)) {
        if ((dma_sniffer_get_data_accumulator() == checksum) && ((uart0_hw->ris & 0x380) == 0))
            serial_uart_reload(false);
        dma_sniffer_disable();
        isIdle = true;
        dma_hw->ints1 = 1 << dma_chan;
    }
}
