/* 
 * File:   matrix.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/gpio.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "hardware/timer.h"
#include "MBI5153/config.h"
#include "Multiplex/Multiplex.h"

test2 buf[2];
volatile uint8_t bank = 0;
static volatile bool stop = false;
static int dma_chan[6];
static Multiplex *m;

static void isr();
static void send_line(uint8_t *line);

void matrix_start() {
    // Init Matrix hardware
    // IO
    for (int i = 0; i < 16; i++) {
        gpio_init(i);
        gpio_set_dir(i, GPIO_OUT);
    }
    for (int i = 0; i < 9; i++)
        gpio_set_function(i, GPIO_FUNC_PIO0);
    gpio_clr_mask(0x7FFF);
    m = Multiplex::getMultiplexer(MULTIPLEX_NUM);
    
    // PIO
    const uint instructions[] = { 
        pio_encode_out(pio_pins, 1) | pio_encode_sideset(1, 0),     // PMP Program
        pio_encode_nop() | pio_encode_sideset(1, 1) 
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

    // Parallel shifter bus
    pio_sm_set_consecutive_pindirs(pio0, 0, 0, 9, true);
    pio0->sm[0].clkdiv = (8 << 16) | (0 << 8);                      // Note: 125MHz / 8 = 15.625MHz - 8 + (0/256)
    pio0->sm[0].pinctrl = (1 << PIO_SM0_PINCTRL_SIDESET_COUNT_LSB) | (8 << PIO_SM0_PINCTRL_OUT_COUNT_LSB) | (8 << PIO_SM0_PINCTRL_SIDESET_BASE_LSB);
    pio0->sm[0].shiftctrl = (1 << PIO_SM0_SHIFTCTRL_AUTOPULL_LSB) | (8 << 25) | (1 << 19);
    pio0->sm[0].execctrl = (1 << 17) | (0x1 << 12);
    hw_set_bits(&pio0->ctrl, 1 << PIO_CTRL_SM_ENABLE_LSB);
    pio0->sm[1].clkdiv = (8 << 16) | (0 << 8);                      // Note: 125MHz / 8 = 15.625MHz - 8 + (0/256)
    pio0->sm[1].pinctrl = (1 << PIO_SM0_PINCTRL_SIDESET_COUNT_LSB) | (8 << PIO_SM0_PINCTRL_OUT_COUNT_LSB) | (8 << PIO_SM0_PINCTRL_SIDESET_BASE_LSB);
    pio0->sm[1].shiftctrl = (1 << PIO_SM0_SHIFTCTRL_AUTOPULL_LSB) | (8 << 25) | (1 << 19);
    pio0->sm[1].execctrl = (1 << 17) | (0x1 << 12);
    hw_set_bits(&pio0->ctrl, 2 << PIO_CTRL_SM_ENABLE_LSB);
    pio0->sm[2].clkdiv = (8 << 16) | (0 << 8);                      // Note: 125MHz / 8 = 15.625MHz - 8 + (0/256)
    pio0->sm[2].pinctrl = (1 << PIO_SM0_PINCTRL_SIDESET_COUNT_LSB) | (8 << PIO_SM0_PINCTRL_OUT_COUNT_LSB) | (8 << PIO_SM0_PINCTRL_SIDESET_BASE_LSB);
    pio0->sm[2].shiftctrl = (1 << PIO_SM0_SHIFTCTRL_AUTOPULL_LSB) | (8 << 25) | (1 << 19);
    pio0->sm[2].execctrl = (1 << 17) | (0x1 << 12);
    hw_set_bits(&pio0->ctrl, 4 << PIO_CTRL_SM_ENABLE_LSB);
    pio0->sm[3].clkdiv = (8 << 16) | (0 << 8);                      // Note: 125MHz / 8 = 15.625MHz - 8 + (0/256)
    pio0->sm[3].pinctrl = (1 << PIO_SM0_PINCTRL_SIDESET_COUNT_LSB) | (8 << PIO_SM0_PINCTRL_OUT_COUNT_LSB) | (8 << PIO_SM0_PINCTRL_SIDESET_BASE_LSB);
    pio0->sm[3].shiftctrl = (1 << PIO_SM0_SHIFTCTRL_AUTOPULL_LSB) | (8 << 25) | (1 << 19);
    pio0->sm[3].execctrl = (1 << 17) | (0x1 << 12);
    hw_set_bits(&pio0->ctrl, 8 << PIO_CTRL_SM_ENABLE_LSB);
    pio1->sm[0].clkdiv = (8 << 16) | (0 << 8);                      // Note: 125MHz / 8 = 15.625MHz - 8 + (0/256)
    pio1->sm[0].pinctrl = (1 << PIO_SM0_PINCTRL_SIDESET_COUNT_LSB) | (8 << PIO_SM0_PINCTRL_OUT_COUNT_LSB) | (8 << PIO_SM0_PINCTRL_SIDESET_BASE_LSB);
    pio1->sm[0].shiftctrl = (1 << PIO_SM0_SHIFTCTRL_AUTOPULL_LSB) | (8 << 25) | (1 << 19);
    pio1->sm[0].execctrl = (1 << 17) | (0x1 << 12);
    hw_set_bits(&pio1->ctrl, 1 << PIO_CTRL_SM_ENABLE_LSB);
    pio1->sm[1].clkdiv = (8 << 16) | (0 << 8);                      // Note: 125MHz / 8 = 15.625MHz - 8 + (0/256)
    pio1->sm[1].pinctrl = (1 << PIO_SM0_PINCTRL_SIDESET_COUNT_LSB) | (8 << PIO_SM0_PINCTRL_OUT_COUNT_LSB) | (8 << PIO_SM0_PINCTRL_SIDESET_BASE_LSB);
    pio1->sm[1].shiftctrl = (1 << PIO_SM0_SHIFTCTRL_AUTOPULL_LSB) | (8 << 25) | (1 << 19);
    pio1->sm[1].execctrl = (1 << 17) | (0x1 << 12);
    hw_set_bits(&pio1->ctrl, 2 << PIO_CTRL_SM_ENABLE_LSB);
    // TODO: Update settings and instructions
    
    // GCLK
    // TODO: Add GCLK and ISR state machine 
    // TODO: Setup pin 10 (GCLK)
    
    // CLK and LAT
    // Add PIO settings and instructions
    
    // DMA
    for (uint8_t i = 0; i < 6; i++) {
        dma_chan[i] = dma_claim_unused_channel(true);
        dma_channel_config c = dma_channel_get_default_config(dma_chan[i]);
        channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
        channel_config_set_read_increment(&c, true);
        channel_config_set_dreq(&c, DREQ_PIO0_TX0);                                                 // TODO: FIX
        dma_channel_configure(dma_chan[i], &c, &pio0_hw->txf[0], NULL, MULTIPLEX * COLUMNS, false); // TODO: FIX
    }
    dma_channel_set_irq0_enabled(dma_chan[5], true);
    irq_set_exclusive_handler(DMA_IRQ_0, isr_data);
    irq_set_priority(DMA_IRQ_0, 1);
    irq_set_enabled(DMA_IRQ_0, true);   
    
    extern void work;
    work();
}

void __not_in_flash_func(send_line)(uint8_t bank_num) {
    dma_hw->ints0 = 1 << dma_chan[5];
    for (uint8_t i = 0; i < 6; i++) {
        uint8_t *line = buf[bank_num][i][0][0];
        dma_channel_set_read_addr(dma_chan[i], line, true);
    }
}

void __not_in_flash_func(isr_data)() {
    stop = true;
    while(stop);
    
    // VSYNC CMD
    uint32_t time = time_us_32();
    // TODO: Make pin 10 (GCLK) GPIO
    while((time_us_32() - time) < 3);
    // TODO: Send VSYNC

    // Dead time
    time = time_us_32();
    // TODO: Set GCLK HIGH
    while((time_us_32() - time) < 1);
    // TODO: Set GCLK LOW
    while((time_us_32() - time) < 3);    
    
    // TODO: Make pin 10 (GCLK) PIO
    // TODO: Kick off hardware to get ISR ticks (GCLK)
}

void __not_in_flash_func(isr_multiplex)() {
    static uint32_t rows = 0;
    static uint8_t flag = bank;
    
    if (stop) {                                                     // Bail if VSYNC is pending
        stop = false;
        return;
    }
    
    uint32_t time = time_us_32();
    m->SetRow(rows);
    
    if (bank != flag) {
        send_line(bank);
        flag = bank;
    }
    
    if (++rows >= MULTIPLEX)
        rows = 0;
    
    while((time_us_32() - time) < 1);                               // Check if timer has expired
    
    // TODO: Kick off hardware to get ISR ticks (GCLK)
}
