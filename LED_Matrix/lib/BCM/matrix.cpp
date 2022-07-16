/* 
 * File:   matrix.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#include <stdint.h>
#include <string.h>
#include "pico/platform.h"
#include "hardware/pio.h"
#include "hardware/gpio.h"
#include "hardware/dma.h"
#include "hardware/timer.h"
#include "BCM/config.h"
#include "Multiplex/Multiplex.h"

test2 buf[2];
volatile uint8_t bank = 0;
static volatile bool stop = false;
static int dma_chan[2];
static Multiplex *m;
static uint8_t *address_table[1 << PWM_bits];
static uint8_t null[COLUMNS + 1];

void matrix_dma_isr();
static void send_line(uint32_t rows, uint8_t buffer);

void matrix_start() {
    // Init Matrix hardware
    // IO
    for (int i = 0; i < 13; i++) {
        gpio_init(i + 8);
        gpio_set_dir(i + 8, GPIO_OUT);
    }
    for (int i = 0; i < 7; i++)
        gpio_set_function(i + 8, GPIO_FUNC_PIO0);
    gpio_init(22);
    gpio_set_dir(22, GPIO_OUT);
    gpio_clr_mask(0x5FFF00);
    m = Multiplex::getMultiplexer(MULTIPLEX_NUM);
    
    memset(buf, COLUMNS, sizeof(buf));
    memset(&null[1], 0, COLUMNS);
    null[0] = COLUMNS;
    
    // Hack to lower the ISR tick rate, accelerates by 2^PWM_bits (Improves refresh performance)
    //  Automates CLK and LAT signals with DMA and PIO to handle Software PWM of entire row
    //      Works like Hardware PWM without the high refresh
    //      This is more or less how it would work with MACHXO2 FPGA and PIC32MX using PMP.
    //          Bus performance is better with RP2040. (Lower cost due to memory, CPU, hardware integration.)
    //  OE is not used in this implementation and held to low to enable the display
    //      Last shift will display display.
    /*while (1) {
        counter2 = (1 << PWM_bits); LAT = 0;    // Start of frame, manually push into FIFO (data stream protocol)
        do {
            counter = COLUMNS;                  // Start of payload, DMA push into FIFO (data stream protocol)
            do {
                DAT = DATA; CLK = 0;            // Payload data, DMA push into FIFO (data stream protocol)
                CLK = 1;                        // Automate CLK pulse
            } while (--counter > 0); CLK = 0;
            LAT = 1;                            // Automate LAT pulse at end of payload (bitplane shift)
            LAT = 0;
        } while (--counter2 > 0);
        IRQ = 1;                                // Call CPU at end of frame
    }*/
    
    // PIO
    const uint16_t instructions[] = { // TODO:
        (uint16_t) (pio_encode_out(pio_pins, 6) | pio_encode_sideset(2, 0)),    // PMP Program
        (uint16_t) (pio_encode_nop() | pio_encode_sideset(2, 1))
    };
    static const struct pio_program pio_programs = {
        .instructions = instructions,
        .length = count_of(instructions),
        .origin = 0,
    };
    pio_add_program(pio0, &pio_programs);
    pio_sm_set_consecutive_pindirs(pio0, 0, 8, 8, true);
    
    // Verify Serial Clock
    constexpr float x2 = SERIAL_CLOCK / (MULTIPLEX * COLUMNS * MAX_REFRESH * (1 << PWM_bits));
    static_assert(x2 >= 1.0);
    constexpr float x = x2 * 125000000.0 / (SERIAL_CLOCK * 2.0);
    static_assert(x >= 1.0);

    // PMP
    pio0->sm[0].clkdiv = ((uint32_t) floor(x) << 16) | ((uint32_t) round((x - floor(x)) * 255.0) << 8);
    pio0->sm[0].pinctrl = (2 << PIO_SM0_PINCTRL_SIDESET_COUNT_LSB) | (6 << PIO_SM0_PINCTRL_OUT_COUNT_LSB) | (14 << PIO_SM0_PINCTRL_SIDESET_BASE_LSB) | 8;
    pio0->sm[0].shiftctrl = (1 << PIO_SM0_SHIFTCTRL_AUTOPULL_LSB) | (6 << 25) | (1 << 19);
    pio0->sm[0].execctrl = (1 << 17) | (0x1 << 12);
    pio0->sm[0].instr = pio_encode_jmp(0);
    hw_set_bits(&pio0->ctrl, 1 << PIO_CTRL_SM_ENABLE_LSB);
    pio_sm_claim(pio0, 0);
    pio_set_irq0_source_enabled(pio0, pis_interrupt0, true);
    
    // DMA
    dma_chan[0] = dma_claim_unused_channel(true);
    dma_chan[1] = dma_claim_unused_channel(true);
    dma_channel_config c = dma_channel_get_default_config(dma_chan[0]);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
    channel_config_set_read_increment(&c, true);
    channel_config_set_dreq(&c, DREQ_PIO0_TX0);
    channel_config_set_chain_to(&c, dma_chan[1]);
    dma_channel_configure(dma_chan[0], &c, &pio0_hw->txf[0], NULL, COLUMNS + 1, false);
    
    c = dma_channel_get_default_config(dma_chan[1]);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_32);
    channel_config_set_read_increment(&c, true);
    dma_channel_configure(dma_chan[1], &c, &dma_hw->ch[dma_chan[0]].al3_read_addr_trig, NULL, 1, false);
    
    send_line(0, 1);
}

void __not_in_flash_func(send_line)(uint32_t rows, uint8_t buffer) {
    for (uint32_t i = 0; i < PWM_bits; i++)
        for (uint32_t k = 0; k < (uint32_t) (1 << i); k++)
            address_table[(1 << i) + k] = buf[buffer][rows][i];
    address_table[(1 << PWM_bits) - 1] = null;
    pio_sm_put(pio0, 0, 1 << PWM_bits);
    dma_channel_set_read_addr(dma_chan[1], address_table, true);
}

void __not_in_flash_func(matrix_dma_isr)() {
    static uint32_t rows = 0;
    
    if (pio_interrupt_get(pio0, 0)) {
        uint64_t time = time_us_64();                                           // Start a timer with 1uS delay                                    
        m->SetRow(rows);
        
        if (++rows >= MULTIPLEX)
            rows = 0;

        while((time_us_64() - time) < BLANK_TIME);                              // Check if timer has expired
        
        send_line(rows, (bank + 1) % 2);                                        // Kick off hardware
        pio_interrupt_clear(pio0, 0);
    }
}

