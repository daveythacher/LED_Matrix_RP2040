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
#include "MBI5153/config.h"
#include "Multiplex/Multiplex.h"

test2 buf[2];
volatile uint8_t bank = 0;
static volatile bool stop = false;
static int dma_chan[6];
static Multiplex *m;
static uint16_t cfg_str[6][COLUMNS / 16];

static void send_cmd(uint8_t cmd);
void matrix_pio_isr();

void matrix_start() {
    // Init Matrix hardware
    // IO
    for (int i = 0; i < 16; i++) {
        gpio_init(i);
        gpio_set_dir(i, GPIO_OUT);
    }
    for (int i = 0; i < 4; i++)
        gpio_set_function(i, GPIO_FUNC_PIO0);
    for (int i = 4; i < 6; i++)
        gpio_set_function(i, GPIO_FUNC_PIO1);
    gpio_set_function(9, GPIO_FUNC_PIO1);
    gpio_set_function(10, GPIO_FUNC_PIO1);
    gpio_set_function(11, GPIO_FUNC_PIO1);
    gpio_clr_mask(0x7FFF);
    m = Multiplex::getMultiplexer(MULTIPLEX_NUM);
    
    // PIO
    const uint instructions[] = { 
        pio_encode_out(pio_pins, 1),    // PMP Program
        pio_encode_irq_wait(false, 4),
        pio_encode_jmp(0),
        pio_encode_pull(false, true),   // CLK/LAT Program
        pio_encode_mov(pio_x, pio_osr) | pio_encode_sideset(2, 0),
        pio_encode_pull(false, true),
        pio_encode_mov(pio_y, pio_osr),
        pio_encode_irq_set(false, 4) | pio_encode_sideset(2, 1),
        pio_encode_nop(),
        pio_encode_irq_clear(false, 4) | pio_encode_sideset(2, 0),
        pio_encode_jmp_x_dec(7),
        pio_encode_irq_set(false, 4) | pio_encode_sideset(2, 3),
        pio_encode_nop(),
        pio_encode_irq_clear(false, 4) | pio_encode_sideset(2, 2),
        pio_encode_jmp_y_dec(11),
        pio_encode_irq_set(false, 0),
        pio_encode_jmp(3),
        pio_encode_pull(false, true),   // GCLK Program
        pio_encode_mov(pio_x, pio_osr),
        pio_encode_nop(),
        pio_encode_jmp_x_dec(19),
        pio_encode_irq_set(false, 1),
        pio_encode_jmp(17)
    };
    for (uint i = 0; i < count_of(instructions); i++) {
        pio0->instr_mem[i] = instructions[i];
        pio1->instr_mem[i] = instructions[i];
    }

/*
    6 PIOs for shifters
    1 PIO for shift clock / latch
    1 PIO for GCLK
*/

    // Verify Serial Clock
    constexpr float x2 = SERIAL_CLOCK / (MULTIPLEX * COLUMNS * FPS * 16.0);
    static_assert(x2 >= 1.0);
    constexpr float x = x2 * 125000000.0 / (SERIAL_CLOCK * 4.0);
    static_assert(x >= 1.0);

    // Parallel shifter bus
    pio_sm_set_consecutive_pindirs(pio0, 0, 0, 9, true);
    pio0->sm[0].clkdiv = ((uint32_t) floor(x) << 16) | ((uint32_t) round((x - floor(x)) * 255.0) << 8);
    pio0->sm[0].pinctrl = (1 << PIO_SM0_PINCTRL_SIDESET_COUNT_LSB) | (8 << PIO_SM0_PINCTRL_OUT_COUNT_LSB) | (8 << PIO_SM0_PINCTRL_SIDESET_BASE_LSB);
    pio0->sm[0].shiftctrl = (1 << PIO_SM0_SHIFTCTRL_AUTOPULL_LSB) | (8 << 25) | (1 << 19);
    pio0->sm[0].execctrl = (1 << 17) | (0x1 << 12);
    hw_set_bits(&pio0->ctrl, 1 << PIO_CTRL_SM_ENABLE_LSB);
    pio0->sm[1].clkdiv = ((uint32_t) floor(x) << 16) | ((uint32_t) round((x - floor(x)) * 255.0) << 8);
    pio0->sm[1].pinctrl = (1 << PIO_SM0_PINCTRL_SIDESET_COUNT_LSB) | (8 << PIO_SM0_PINCTRL_OUT_COUNT_LSB) | (8 << PIO_SM0_PINCTRL_SIDESET_BASE_LSB);
    pio0->sm[1].shiftctrl = (1 << PIO_SM0_SHIFTCTRL_AUTOPULL_LSB) | (8 << 25) | (1 << 19);
    pio0->sm[1].execctrl = (1 << 17) | (0x1 << 12);
    hw_set_bits(&pio0->ctrl, 2 << PIO_CTRL_SM_ENABLE_LSB);
    pio0->sm[2].clkdiv = ((uint32_t) floor(x) << 16) | ((uint32_t) round((x - floor(x)) * 255.0) << 8);
    pio0->sm[2].pinctrl = (1 << PIO_SM0_PINCTRL_SIDESET_COUNT_LSB) | (8 << PIO_SM0_PINCTRL_OUT_COUNT_LSB) | (8 << PIO_SM0_PINCTRL_SIDESET_BASE_LSB);
    pio0->sm[2].shiftctrl = (1 << PIO_SM0_SHIFTCTRL_AUTOPULL_LSB) | (8 << 25) | (1 << 19);
    pio0->sm[2].execctrl = (1 << 17) | (0x1 << 12);
    hw_set_bits(&pio0->ctrl, 4 << PIO_CTRL_SM_ENABLE_LSB);
    pio0->sm[3].clkdiv = ((uint32_t) floor(x) << 16) | ((uint32_t) round((x - floor(x)) * 255.0) << 8);
    pio0->sm[3].pinctrl = (1 << PIO_SM0_PINCTRL_SIDESET_COUNT_LSB) | (8 << PIO_SM0_PINCTRL_OUT_COUNT_LSB) | (8 << PIO_SM0_PINCTRL_SIDESET_BASE_LSB);
    pio0->sm[3].shiftctrl = (1 << PIO_SM0_SHIFTCTRL_AUTOPULL_LSB) | (8 << 25) | (1 << 19);
    pio0->sm[3].execctrl = (1 << 17) | (0x1 << 12);
    hw_set_bits(&pio0->ctrl, 8 << PIO_CTRL_SM_ENABLE_LSB);
    pio1->sm[0].clkdiv = ((uint32_t) floor(x) << 16) | ((uint32_t) round((x - floor(x)) * 255.0) << 8);
    pio1->sm[0].pinctrl = (1 << PIO_SM0_PINCTRL_SIDESET_COUNT_LSB) | (8 << PIO_SM0_PINCTRL_OUT_COUNT_LSB) | (8 << PIO_SM0_PINCTRL_SIDESET_BASE_LSB);
    pio1->sm[0].shiftctrl = (1 << PIO_SM0_SHIFTCTRL_AUTOPULL_LSB) | (8 << 25) | (1 << 19);
    pio1->sm[0].execctrl = (1 << 17) | (0x1 << 12);
    hw_set_bits(&pio1->ctrl, 1 << PIO_CTRL_SM_ENABLE_LSB);
    pio1->sm[1].clkdiv = ((uint32_t) floor(x) << 16) | ((uint32_t) round((x - floor(x)) * 255.0) << 8);
    pio1->sm[1].pinctrl = (1 << PIO_SM0_PINCTRL_SIDESET_COUNT_LSB) | (8 << PIO_SM0_PINCTRL_OUT_COUNT_LSB) | (8 << PIO_SM0_PINCTRL_SIDESET_BASE_LSB);
    pio1->sm[1].shiftctrl = (1 << PIO_SM0_SHIFTCTRL_AUTOPULL_LSB) | (8 << 25) | (1 << 19);
    pio1->sm[1].execctrl = (1 << 17) | (0x1 << 12);
    hw_set_bits(&pio1->ctrl, 2 << PIO_CTRL_SM_ENABLE_LSB);
    // TODO: Update settings and instructions
    
    // CLK and LAT
    // TODO: Add PIO settings and instructions
    
    // GCLK
    // TODO: Add GCLK and ISR state machine 
    // TODO: Setup pin 10 (GCLK)
    
    pio_set_irq0_source_enabled(pio1, pis_interrupt0, true);
    pio_set_irq0_source_enabled(pio1, pis_interrupt1, true);
    
    // DMA
    for (int i = 0; i < 6; i++)
        dma_chan[i] = dma_claim_unused_channel(true);
        
    dma_channel_config c = dma_channel_get_default_config(dma_chan[0]);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_16);
    channel_config_set_read_increment(&c, true);
    channel_config_set_dreq(&c, DREQ_PIO0_TX0);
    dma_channel_configure(dma_chan[0], &c, &pio0_hw->txf[0], NULL, COLUMNS / 16, false);
    c = dma_channel_get_default_config(dma_chan[1]);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_16);
    channel_config_set_read_increment(&c, true);
    channel_config_set_dreq(&c, DREQ_PIO0_TX1);
    dma_channel_configure(dma_chan[1], &c, &pio0_hw->txf[1], NULL, COLUMNS / 16, false);
    c = dma_channel_get_default_config(dma_chan[2]);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_16);
    channel_config_set_read_increment(&c, true);
    channel_config_set_dreq(&c, DREQ_PIO0_TX2);
    dma_channel_configure(dma_chan[2], &c, &pio0_hw->txf[2], NULL, COLUMNS / 16, false);
    c = dma_channel_get_default_config(dma_chan[3]);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_16);
    channel_config_set_read_increment(&c, true);
    channel_config_set_dreq(&c, DREQ_PIO0_TX3);
    dma_channel_configure(dma_chan[3], &c, &pio0_hw->txf[3], NULL, COLUMNS / 16, false);
    c = dma_channel_get_default_config(dma_chan[4]);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_16);
    channel_config_set_read_increment(&c, true);
    channel_config_set_dreq(&c, DREQ_PIO1_TX0);
    dma_channel_configure(dma_chan[4], &c, &pio1_hw->txf[0], NULL, COLUMNS / 16, false);
    c = dma_channel_get_default_config(dma_chan[5]);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_16);
    channel_config_set_read_increment(&c, true);
    channel_config_set_dreq(&c, DREQ_PIO1_TX1);
    dma_channel_configure(dma_chan[5], &c, &pio1_hw->txf[1], NULL, COLUMNS / 16, false);
     
    memset(buf, 0, sizeof(buf));
    
    // MBI5153 Configuration
    send_cmd(14);
    for (uint16_t i = 0; i < (COLUMNS / 16); i++) {
        cfg_str[0][i] = (MULTIPLEX - 1) << 8;
        cfg_str[1][i] = (MULTIPLEX - 1) << 8;
        cfg_str[2][i] = (MULTIPLEX - 1) << 8;
        cfg_str[3][i] = (MULTIPLEX - 1) << 8;
        cfg_str[4][i] = (MULTIPLEX - 1) << 8;
        cfg_str[5][i] = (MULTIPLEX - 1) << 8;
    }
    for (uint8_t i = 0; i < 6; i++)
        dma_channel_set_read_addr(dma_chan[i], (uint8_t *) cfg_str[i], true);
    send_cmd(4);
    
    // Interrupt from CFG1 write will trigger ISR ticks
    pio_sm_put(pio1, 3, 512);
}

void __not_in_flash_func(send_cmd)(uint8_t cmd) {
    pio_sm_put(pio1, 2, COLUMNS - cmd);
    pio_sm_put(pio1, 2, cmd);
}

void __not_in_flash_func(matrix_pio_isr0)() {    
    static uint8_t rows = 0;
    static uint8_t counter = 0;
    
    if (pio_interrupt_get(pio1, 0)) {
        if (++counter >= (COLUMNS / 16)) {
            counter = 1;
            if (++rows >= MULTIPLEX) {
                uint32_t time = time_us_32();
                rows = 1; 
                gpio_init(10);
                while (time_us_32() - time < 3);                    // Wait 50 GCLKs
                stop = true;
                while(stop);
                send_cmd(2);                                        // Send VSYNC CMD

                // Dead time
                time = time_us_32();
                // TODO: Set GCLK HIGH
                while((time_us_32() - time) < 1);
                // TODO: Set GCLK LOW
                while((time_us_32() - time) < 3);    
                    
                // Kick off hardware to get ISR ticks (GCLK)
                gpio_set_function(10, GPIO_FUNC_PIO1);
                pio_sm_put(pio1, 3, 512);
            }
        }
        
        for (int i = 0; i < 6; i++)
            dma_channel_set_read_addr(dma_chan[i], (uint8_t *) &buf[bank][i][rows - 1][counter - 1], true);

        send_cmd(1);
        pio_interrupt_clear(pio1, 0);
    }
}

void __not_in_flash_func(matrix_pio_isr1)() {
    static uint32_t rows = 1;
    
    if (pio_interrupt_get(pio1, 1)) {
        uint32_t time = time_us_32();
        
        if (stop) {                                                 // Bail if VSYNC is pending
            stop = false;
            return pio_interrupt_clear(pio1, 1);
        }
        
        m->SetRow(rows);
        if (++rows >= MULTIPLEX)
            rows = 0;
        
        while((time_us_32() - time) < BLANK_TIME);                  // Check if timer has expired
        
        // Kick off hardware to get ISR ticks (GCLK)
        pio_sm_put(pio1, 3, 512);
        pio_interrupt_clear(pio1, 1);
    }
}
