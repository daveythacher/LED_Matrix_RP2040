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

namespace Serial {
    static int dma_chan[2];
    static dma_channel_config c[2];
    static volatile bool isIdle = true;
    static volatile uint32_t checksum;

    static void uart_reload(bool reload_dma);

    #if __BYTE_ORDER == __LITTLE_ENDIAN
        #define ntohs(x) __bswap16(x)
        #define ntohl(x) __bswap32(x)
    #else
        #define ntohs(x) ((uint16_t)(x))
        #define ntoh1(x) ((uint32_t)(x))
    #endif

    void uart_start(int dma0, int dma1) {
        dma_chan[0] = dma0;
        dma_chan[1] = dma1;

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
        c[0] = dma_channel_get_default_config(dma_chan[0]);
        channel_config_set_transfer_data_size(&c[0], DMA_SIZE_8);
        channel_config_set_write_increment(&c[0], true);
        channel_config_set_read_increment(&c[0], false);
        channel_config_set_dreq(&c[0], DREQ_UART0_RX);
        channel_config_set_chain_to(&c[0], dma_chan[1]);
        dma_channel_set_irq1_enabled(dma_chan[0], false);
        c[1] = dma_channel_get_default_config(dma_chan[1]);
        channel_config_set_transfer_data_size(&c[1], DMA_SIZE_8);
        channel_config_set_write_increment(&c[1], true);
        channel_config_set_read_increment(&c[1], false);
        channel_config_set_dreq(&c[1], DREQ_UART0_RX);
        dma_channel_set_irq1_enabled(dma_chan[1], false);

        uart_reload(false);
    }


    // Note: Response time of seeing start token
    //  The sender should wait for the bus to go active before sending checksum and data.
    //  However the sender can get a false positive if this task is starved. (ISR can starve.)
    //    Sender must wait long enough to ensure this cannot happen!
    // NOTE: Current implementation works but may have issues with certain configuration settings
    //  Cannot use high baud rates, high color density, high refresh rates and/or large blank times
    void __not_in_flash_func(uart_task)() {
        static uint64_t time = time_us_64(); 

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
                isIdle = false;
                uart_reload(true);
            }
        }

        // Note this is allowed to trip the error recovery protocol, which should not cause an issue.
        if (dma_channel_get_irq1_status(dma_chan[1])) {
            if ((dma_sniffer_get_data_accumulator() == checksum) && ((uart0_hw->ris & 0x380) == 0))
                uart_reload(false);
            dma_sniffer_disable();
            isIdle = true;
            dma_hw->ints1 = 1 << dma_chan[0];
            dma_hw->ints1 = 1 << dma_chan[1];
        }
    }

    void __not_in_flash_func(uart_reload)(bool reload_dma) {
        static uint8_t *ptr = 0;
        static uint8_t *buf = 0;
        static uint16_t len = 0;
        uint16_t *p = (uint16_t *) buf;
        
        // Note: Uses CRC-32/MPEG-2
        if (reload_dma) {
            dma_sniffer_set_data_accumulator(0xFFFFFFFF);
            dma_channel_configure(dma_chan[0], &c[0], &checksum, &uart_get_hw(uart0)->dr, sizeof(checksum), true);
            dma_channel_configure(dma_chan[1], &c[1], buf, &uart_get_hw(uart0)->dr, len, false);
            dma_sniffer_enable(dma_chan[1], 0, true);
        }
        else {
            if ((p != 0) && (sizeof(DEFINE_SERIAL_RGB_TYPE) >= 2)) {
                for (uint16_t i = 0; i < len; i += 2)
                    p[i / 2] = ntohs(p[i / 2]);
            }

            uart_callback(&buf, &len);

            if (ptr)
                Matrix::Worker::process((void *) ptr, false);
        }
    }
}
