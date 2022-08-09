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
#include "hardware/structs/bus_ctrl.h"
#include "Matrix/config.h"
#include "Matrix/matrix.h"
#include "Matrix/TLC5958/memory_format.h"
#include "Multiplex/Multiplex.h"

test2 buf[3];
static uint8_t bank = 0;
static int dma_chan;
static Multiplex *m;
static bool isFinished = false;

void matrix_start() {
    // Init Matrix hardware
    // IO
    for (int i = 0; i < 13; i++) {
        gpio_init(i + 8);
        gpio_set_dir(i + 8, GPIO_OUT);
    }
    for (int i = 0; i < 8; i++)
        gpio_set_function(i + 8, GPIO_FUNC_PIO0);
    gpio_init(22);
    gpio_set_dir(22, GPIO_OUT);
    gpio_clr_mask(0x5FFF00);
    m = Multiplex::getMultiplexer(MULTIPLEX_NUM);
    
    /*
        1 - CLK/LAT
        1 - DAT
        1 - GCLK
    */
    
    // PIO
    const uint16_t instructions[] = {
        (uint16_t) (pio_encode_pull(false, true) | pio_encode_sideset(2, 0)),   // PIO SM
        (uint16_t) (pio_encode_mov(pio_x, pio_osr) | pio_encode_sideset(2, 0)),
        (uint16_t) (pio_encode_out(pio_null, 24) | pio_encode_sideset(2, 0)),
        (uint16_t) (pio_encode_pull(false, true) | pio_encode_sideset(2, 0)),
        (uint16_t) (pio_encode_out(pio_null, 24) | pio_encode_sideset(2, 0)),
        (uint16_t) (pio_encode_mov(pio_y, pio_osr) | pio_encode_sideset(2, 0)),
        (uint16_t) (pio_encode_out(pio_pins, 6) | pio_encode_sideset(2, 0)),    // PMP Program
        (uint16_t) (pio_encode_jmp_y_dec(6) | pio_encode_sideset(2, 1)),
        (uint16_t) (pio_encode_nop() | pio_encode_sideset(2, 2)),
        (uint16_t) (pio_encode_nop() | pio_encode_sideset(2, 2)),
        (uint16_t) (pio_encode_nop() | pio_encode_sideset(2, 0)),
        (uint16_t) (pio_encode_jmp_x_dec(3) | pio_encode_sideset(2, 0)),
        (uint16_t) (pio_encode_jmp(0) | pio_encode_sideset(2, 0))
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

    // PMP / SM
    pio0->sm[0].clkdiv = ((uint32_t) floor(x) << 16) | ((uint32_t) round((x - floor(x)) * 255.0) << 8);
    pio0->sm[0].pinctrl = (2 << PIO_SM0_PINCTRL_SIDESET_COUNT_LSB) | (6 << PIO_SM0_PINCTRL_OUT_COUNT_LSB) | (14 << PIO_SM0_PINCTRL_SIDESET_BASE_LSB) | 8;
    pio0->sm[0].shiftctrl = (1 << PIO_SM0_SHIFTCTRL_AUTOPULL_LSB) | (6 << 25) | (1 << 19);
    pio0->sm[0].execctrl = (1 << 17) | (12 << 12);
    pio0->sm[0].instr = pio_encode_jmp(0);
    hw_set_bits(&pio0->ctrl, 1 << PIO_CTRL_SM_ENABLE_LSB);
    pio_sm_claim(pio0, 0);
    
    // DMA
    bus_ctrl_hw->priority = (1 << 12) | (1 << 8);
    dma_chan = dma_claim_unused_channel(true);
    
    // DAT DMA
    dma_channel_config c = dma_channel_get_default_config(dma_chan);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
    channel_config_set_read_increment(&c, true);
    channel_config_set_dreq(&c, DREQ_PIO0_TX0);
    dma_channel_configure(dma_chan, &c, &pio0_hw->txf[0], NULL, COLUMNS / 16 * 3, false);
    dma_channel_set_irq0_enabled(dma_chan, true); 
    
    extern void matrix_fifo_isr_0();
    irq_set_exclusive_handler(DMA_SIO_0, matrix_fifo_isr_0);
    irq_set_priority(DMA_SIO_0, 0xFF);                                          // Let anything preempt this!
    irq_set_enabled(DMA_SIO_0, true);
    
    // TODO:                                                                    // Kick off hardware (GCLK)
}

void __not_in_flash_func(matrix_dma_isr)() {    
    if (dma_channel_get_irq0_status(dma_chan)) {
        isFinished = true;
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
            //TODO:                                                             // Kick off hardware (CLK)
        }
        if (isFinished) {
            // TODO:                                                            // VSYNC Procedure
            return;
        }
    }
    m->SetRow(rows);

    while((time_us_64() - time) < BLANK_TIME);                                  // Check if timer has expired
    // TODO:                                                                    // Kick off hardware (GCLK)
}

