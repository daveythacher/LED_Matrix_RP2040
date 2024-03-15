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
#include "Serial/helper.h"

namespace Serial {
    static int dma_chan[2];
    static dma_channel_config c[2];
    static char state = 'y';
    static int sm;
    static const int trigger = 2;
    static const int sync = 3;

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
        gpio_init(trigger);
        gpio_init(sync);
        gpio_set_dir(0, GPIO_OUT);
        gpio_set_dir(trigger, GPIO_OUT);
        gpio_set_function(0, GPIO_FUNC_UART);
        gpio_set_function(1, GPIO_FUNC_UART);

        // UART
        static_assert(SERIAL_UART_BAUD <= 7800000, "Baud rate must be less than 7.8MBaud");
        uart_init(uart0, SERIAL_UART_BAUD);

        // PIO
        sm = pio_claim_unused_sm(pio1, true);
        APP::setup_debouncer(pio1, sm, sync);

        // DMA
        c[0] = dma_channel_get_default_config(dma_chan[0]);
        channel_config_set_transfer_data_size(&c[0], DMA_SIZE_8);
        channel_config_set_write_increment(&c[0], true);
        channel_config_set_read_increment(&c[0], false);
        channel_config_set_dreq(&c[0], DREQ_UART0_RX);
        channel_config_set_chain_to(&c[0], dma_chan[1]);

        c[1] = dma_channel_get_default_config(dma_chan[1]);
        channel_config_set_transfer_data_size(&c[1], DMA_SIZE_8);
        channel_config_set_write_increment(&c[1], true);
        channel_config_set_read_increment(&c[1], false);
        channel_config_set_dreq(&c[1], DREQ_UART0_RX);

        uart_reload(false);             // Prepare next frame
        uart_reload(true);              // Start DMA state machine
    }

    // Warning host is required to obey flow control and handle bus recovery
    // Protocol overview: (Not finished)
    //    1. Data is received on RX (assumes 7.8 MBaud)
    //        a. Recovery protocol (Needs work)
    //            i. Push byte slowly (10mS) till ack byte moves from 'n' to 'y'.
    //            ii. Continue till ack byte 'y' moves to 'n', set count to 1.
    //            iii. Continue till ack byte is 'y' again, counting each push.
    //            iv. Push the counted number of bytes
    //    2. Ack byte (flow control) is sent on TX (periodically - 10uS)
    //        a. First half of frame will send 'n' to indicate the bus is active.
    //        b. Second half of frame will send 'y' to indicate the bus is idle. (Note it may still be completing second half.)
    //    3. Trigger signal is used to notify the controllers that they have reached the fence.
    //    4. Sync signal is used to notify the controllers that all have reached the fence. (All trigger signals anded together.)
    //        a. This signal should be at least +/- 100uS, transmitter to receiver.
    void __not_in_flash_func(uart_task)() {
        static uint64_t time = 0;
        static bool isReady = false;

        // Check for errors
        if (!((uart0_hw->ris & 0x380) == 0)) {
            uart0_hw->icr = 0x7FF;
        }

        // First half of packet completed (bus still active)
        if (dma_channel_get_irq1_status(dma_chan[0])) {
            state = 'n';
            dma_hw->ints1 = 1 << dma_chan[0];
        }

        // Second half of packet completed (bus now idle)
        if (dma_channel_get_irq1_status(dma_chan[1])) {
            state = 'y';
            isReady = true;
            dma_hw->ints1 = 1 << dma_chan[1];
        }

        if (isReady) {
            gpio_set_mask(1 << trigger);
            
            if (pio1_hw->intr & 1 << (sm + 8)) {
                uart_reload(false);
                gpio_clr_mask(1 << trigger);
                isReady = false;
                uart_reload(true);
                pio1_hw->irq = 1 << sm;
            }
        }

        // Report flow control and/or internal state
        if ((time + 10) < time_us_64()) {
            uart_putc(uart0, state);
            time = time_us_64();
        }
    }

    void __not_in_flash_func(uart_reload)(bool reload_dma) {
        static uint8_t *buf = 0;
        static uint16_t len = 0;
        uint16_t *p = (uint16_t *) buf;
        
        // Packet state machine for frame
        if (reload_dma) {
            uint32_t size = len / 2;

            dma_channel_configure(dma_chan[1], &c[1], &buf[size], &uart_get_hw(uart0)->dr, size, false);
            dma_channel_configure(dma_chan[0], &c[0], &buf[0], &uart_get_hw(uart0)->dr, size, true);
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
