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
        dma_channel_set_irq1_enabled(dma_chan[0], false);

        // TODO:

        uart_reload(false);
    }

    void __not_in_flash_func(uart_task)() {
        // Check for errors
        if (!((uart0_hw->ris & 0x380) == 0)) {
            uart0_hw->icr = 0x7FF;
        }

        // Check DMA state, abort if required
        if (dma_channel_get_irq1_status(dma_chan[0])) {
            // TODO:
            //uart_reload(false);
            dma_hw->ints1 = 1 << dma_chan[0];
        }

        {
            // TODO:
            uart_reload(true);
        }
    }

    void __not_in_flash_func(uart_reload)(bool reload_dma) {
        static uint8_t *buf = 0;
        static uint16_t len = 0;
        uint16_t *p = (uint16_t *) buf;
        
        if (reload_dma) {
            dma_channel_configure(dma_chan[0], &c[0], buf, &uart_get_hw(uart0)->dr, len, true);
        }
        else {
            switch (sizeof(DEFINE_SERIAL_RGB_TYPE)) {
                case 2:
                case 6:
                    for (uint16_t i = 0; i < len; i += 2)
                        p[i / 2] = ntohs(p[i / 2]);
                    break;
                default:
                    break;
            }

            if (buf) {
                if (isPacket)
                    Matrix::Worker::process((void *) buf);
                else
                    Matrix::Loafer::toss((void *) buf);
            }

            uart_callback(&buf, &len);
        }
    }
}
