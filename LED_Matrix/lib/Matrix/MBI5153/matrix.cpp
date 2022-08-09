/* 
 * File:   matrix.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#include <stdint.h>
#include <string.h>
#include <algorithm>
#include "pico/platform.h"
#include "hardware/pio.h"
#include "hardware/gpio.h"
#include "hardware/dma.h"
#include "hardware/timer.h"
#include "hardware/irq.h"
#include "hardware/structs/bus_ctrl.h"
#include "Matrix/config.h"
#include "Matrix/matrix.h"
#include "Matrix/GEN3/memory_format.h"
#include "Multiplex/Multiplex.h"

test2 buf[3];
static uint8_t bank = 0;
static int dma_chan[6];
static Multiplex *m;
static bool isFinished = false;
static const uint8_t lat_cmd = 2;
static const uint8_t seg_bits = 7;

static void start_clk(uint8_t cmd);
static void start_gclk(uint8_t bits);

void matrix_start() {
    // Init Matrix hardware
    // IO
    for (int i = 0; i < 13; i++) {
        gpio_init(i + 8);
        gpio_set_dir(i + 8, GPIO_OUT);
    }
    gpio_init(22);
    gpio_set_dir(22, GPIO_OUT);
    gpio_set_function(8, GPIO_FUNC_PIO0);
    gpio_set_function(9, GPIO_FUNC_PIO0);
    gpio_set_function(10, GPIO_FUNC_PIO0);
    gpio_set_function(11, GPIO_FUNC_PIO0);
    gpio_set_function(12, GPIO_FUNC_PIO1);
    gpio_set_function(13, GPIO_FUNC_PIO1);
    gpio_set_function(14, GPIO_FUNC_PIO1);
    gpio_set_function(15, GPIO_FUNC_PIO1);
    gpio_set_function(22, GPIO_FUNC_PIO1);
    gpio_clr_mask(0x5FFF00);
    m = Multiplex::getMultiplexer(MULTIPLEX_NUM);
    
    /*
        1 - CLK/LAT
        6 - DAT
        1 - GCLK
    */
    
    // PIO
    const uint16_t instructions[] = {
        (uint16_t) (pio_encode_out(pio_pins, 1)),                               // DAT Program (125 MHz Clock)
        (uint16_t) (pio_encode_wait_gpio(true, 14)),
        (uint16_t) (pio_encode_wait_gpio(false, 14)),
        (uint16_t) (pio_encode_pull(false, true) | pio_encode_sideset(2, 0)),   // CLK/LAT Program (Cannot exceed 25MHz Clock)
        (uint16_t) (pio_encode_mov(pio_x, pio_osr) | pio_encode_sideset(2, 0)),
        (uint16_t) (pio_encode_pull(false, true) | pio_encode_sideset(2, 0)),
        (uint16_t) (pio_encode_mov(pio_y, pio_osr) | pio_encode_sideset(2, 0)),
        (uint16_t) (pio_encode_nop() | pio_encode_sideset(2, 0)),
        (uint16_t) (pio_encode_jmp_y_dec(7) | pio_encode_sideset(2, 1)),
        (uint16_t) (pio_encode_nop() | pio_encode_sideset(2, 0)),
        (uint16_t) (pio_encode_nop() | pio_encode_sideset(2, 2)),
        (uint16_t) (pio_encode_nop() | pio_encode_sideset(2, 2)),
        (uint16_t) (pio_encode_jmp_x_dec(11) | pio_encode_sideset(2, 3)),
        (uint16_t) (pio_encode_nop() | pio_encode_sideset(2, 2)),
        (uint16_t) (pio_encode_pull(false, true) | pio_encode_sideset(1, 0)),   // GCLK Program (Cannot exceed 33MHz Clock)
        (uint16_t) (pio_encode_mov(pio_x, pio_osr) | pio_encode_sideset(1, 0)),
        (uint16_t) (pio_encode_nop() | pio_encode_sideset(1, 0)),
        (uint16_t) (pio_encode_jmp_x_dec(16) | pio_encode_sideset(1, 1)),
        (uint16_t) (pio_encode_nop() | pio_encode_sideset(1, 0))
    };
    static const struct pio_program pio_programs = {
        .instructions = instructions,
        .length = count_of(instructions),
        .origin = 0,
    };
    pio_add_program(pio0, &pio_programs);
    pio_add_program(pio1, &pio_programs);
    pio_sm_set_consecutive_pindirs(pio0, 0, 8, 1, true);
    pio_sm_set_consecutive_pindirs(pio0, 1, 9, 1, true);
    pio_sm_set_consecutive_pindirs(pio0, 2, 10, 1, true);
    pio_sm_set_consecutive_pindirs(pio0, 3, 11, 1, true);
    pio_sm_set_consecutive_pindirs(pio1, 0, 12, 1, true);
    pio_sm_set_consecutive_pindirs(pio1, 1, 13, 1, true);
    pio_sm_set_consecutive_pindirs(pio1, 2, 14, 2, true);
    pio_sm_set_consecutive_pindirs(pio1, 3, 22, 1, true);
    
    // Verify Serial Clock
    static_assert(SERIAL_CLOCK <= 33000000.0);
    
    constexpr float x = (SERIAL_CLOCK * 0.75) / (MULTIPLEX * COLUMNS * FPS * 16);
    static_assert(x >= 1.0);
    constexpr float CLK = x * 125000000.0 / (SERIAL_CLOCK * 2.0 * 0.75);
    static_assert(CLK >= 1.0);
    
    constexpr float x2 = SERIAL_CLOCK / (MULTIPLEX * MAX_REFRESH * (1 << std::max(PWM_bits, (int) seg_bits)));
    static_assert(x2 >= 1.0);
    constexpr float GCLK = x2 * 125000000.0 / (SERIAL_CLOCK * 2.0);
    static_assert(GCLK >= 1.0);

    // DAT SM
    for (int i = 0; i < 4; i++) {
        pio0->sm[i].clkdiv = 0;
        pio0->sm[i].execctrl = (1 << 17) | (2 << 12);
        pio0->sm[i].shiftctrl = (1 << PIO_SM0_SHIFTCTRL_AUTOPULL_LSB) | (16 << 25) | (1 << 19);
        pio0->sm[i].pinctrl = (1 << PIO_SM0_PINCTRL_OUT_COUNT_LSB) | (8 + i);
        pio0->sm[i].instr = pio_encode_jmp(0);
        hw_set_bits(&pio0->ctrl, 1 << (PIO_CTRL_SM_ENABLE_LSB + i));
        pio_sm_claim(pio0, i);
    }
    
    for (int i = 0; i < 2; i++) {
        pio1->sm[i].clkdiv = 0;
        pio1->sm[i].execctrl = (1 << 17) | (2 << 12);
        pio1->sm[i].shiftctrl = (1 << PIO_SM0_SHIFTCTRL_AUTOPULL_LSB) | (16 << 25) | (1 << 19);
        pio1->sm[i].pinctrl = (1 << PIO_SM0_PINCTRL_OUT_COUNT_LSB) | (12 + i);
        pio1->sm[i].instr = pio_encode_jmp(0);
        hw_set_bits(&pio1->ctrl, 1 << (PIO_CTRL_SM_ENABLE_LSB + i));
        pio_sm_claim(pio1, i);
    }

    // CLK/LAT SM
    pio1->sm[1].clkdiv = ((uint32_t) floor(CLK) << 16) | ((uint32_t) round((CLK - floor(CLK)) * 255.0) << 8);
    pio1->sm[1].execctrl = (13 << 12);
    pio1->sm[1].shiftctrl = 0;
    pio1->sm[1].pinctrl = (2 << PIO_SM0_PINCTRL_SIDESET_COUNT_LSB) | (14 << PIO_SM0_PINCTRL_SIDESET_BASE_LSB);
    pio1->sm[1].instr = pio_encode_jmp(3);
    hw_set_bits(&pio1->ctrl, 4 << PIO_CTRL_SM_ENABLE_LSB);
    pio_sm_claim(pio1, 2);

    // GCLK SM
    pio1->sm[2].clkdiv = ((uint32_t) floor(GCLK) << 16) | ((uint32_t) round((GCLK - floor(GCLK)) * 255.0) << 8);
    pio1->sm[2].execctrl = (18 << 12);
    pio1->sm[2].shiftctrl = 0;
    pio1->sm[2].pinctrl = (1 << PIO_SM0_PINCTRL_SIDESET_COUNT_LSB) | (22 << PIO_SM0_PINCTRL_SIDESET_BASE_LSB);
    pio1->sm[2].instr = pio_encode_jmp(14);
    hw_set_bits(&pio1->ctrl, 8 << PIO_CTRL_SM_ENABLE_LSB);
    pio_sm_claim(pio1, 3);
    
    // DMA
    bus_ctrl_hw->priority = (1 << 12) | (1 << 8);
    for (int i = 0; i < 6; i++)
        dma_chan[i] = dma_claim_unused_channel(true);
    
    // DAT DMA
    dma_channel_config c;
    for (int i = 0; i < 6; i++) {
        c = dma_channel_get_default_config(dma_chan[i]);
        channel_config_set_transfer_data_size(&c, DMA_SIZE_16);
        channel_config_set_read_increment(&c, true);
        channel_config_set_dreq(&c, DREQ_PIO0_TX0);
        dma_channel_configure(dma_chan[i], &c, &pio0_hw->txf[0], NULL, COLUMNS / 16, false);
    }
    
    // TODO: Boot procedure
    
    dma_channel_set_irq0_enabled(dma_chan[5], true); 
    
    extern void matrix_fifo_isr_0();
    irq_set_exclusive_handler(SIO_IRQ_PROC0, matrix_fifo_isr_0);
    irq_set_priority(SIO_IRQ_PROC0, 0xFF);                                      // Let anything preempt this!
    irq_set_enabled(SIO_IRQ_PROC0, true);
    
    start_gclk(seg_bits);                                                       // Kick off hardware (GCLK)
}

// Keep this short and sweet!
void __not_in_flash_func(matrix_dma_isr)() { 
    static uint16_t counter = 0;
       
    if (dma_channel_get_irq0_status(dma_chan[5])) {
        if (++counter >= (MULTIPLEX * 16)) {                                    // Fire rate: MULTIPLEX * 16 * FPS
            isFinished = true;
            counter = 0;
        }
        else
            start_clk(lat_cmd);
        dma_hw->ints0 = 1 << dma_chan[5];
    }
}

void __not_in_flash_func(matrix_gclk_task)() {
    static uint32_t rows = 0;
    extern volatile bool vsync;
    uint64_t time = time_us_64();                                               // Start a timer with 1uS delay
        
    if (++rows >= MULTIPLEX) {
        rows = 0;
        if (vsync) {
            bank = (bank + 1) % 3;
            vsync = false;
            start_clk(lat_cmd);                                                 // Kick off hardware (CLK)
        }
        if (isFinished) {
            // TODO:                                                            // VSYNC Procedure
            return;
        }
    }
    m->SetRow(rows);

    while((time_us_64() - time) < BLANK_TIME);                                  // Check if timer has expired
    start_gclk(seg_bits);                                                       // Kick off hardware (GCLK)
}

void start_clk(uint8_t cmd) {
    // TODO: Reload DMA(s)
    // TODO: Reload PIO (CLK/LAT)
}

void start_gclk(uint8_t bits) {
    // TODO: Reload PIO (GCLK)
}

