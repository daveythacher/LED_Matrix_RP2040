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
#include "hardware/structs/bus_ctrl.h"
#include "Matrix/config.h"
#include "Matrix/matrix.h"
#include "Matrix/TLC5958/memory_format.h"
#include "Multiplex/Multiplex.h"

test2 buf[3];
static uint8_t bank = 0;
static int dma_chan;
static bool isFinished = false;
static const uint8_t lat_cmd = 1;
static const uint8_t seg_bits = 8;

static void start_clk(uint16_t counter, uint8_t cmd);
static void start_gclk(uint8_t bits);
static void send_cmd(uint8_t cmd);

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
    gpio_set_function(22, GPIO_FUNC_PIO0);
    gpio_clr_mask(0x5FFF00);
    
    /*
        1 - CLK/LAT
        1 - DAT
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
        (uint16_t) (pio_encode_jmp_x_dec(7) | pio_encode_sideset(2, 1)),
        (uint16_t) (pio_encode_nop() | pio_encode_sideset(2, 0)),
        (uint16_t) (pio_encode_nop() | pio_encode_sideset(2, 2)),
        (uint16_t) (pio_encode_nop() | pio_encode_sideset(2, 2)),
        (uint16_t) (pio_encode_jmp_y_dec(11) | pio_encode_sideset(2, 3)),
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
    pio_sm_set_consecutive_pindirs(pio0, 0, 8, 1, true);
    pio_sm_set_consecutive_pindirs(pio0, 1, 14, 2, true);
    pio_sm_set_consecutive_pindirs(pio0, 2, 22, 1, true);
    
    // Verify Serial Clock
    static_assert(SERIAL_CLOCK <= 33000000.0);
    
    constexpr float x = (SERIAL_CLOCK * 0.75) / (MULTIPLEX * COLUMNS * FPS * 16 * 3);
    static_assert(x >= 1.0, "SERIAL_CLOCK too low");
    constexpr float CLK = x * 125000000.0 / (SERIAL_CLOCK * 2.0 * 0.75);
    static_assert(CLK >= 1.0, "Unabled to configure PIO for SERIAL_CLOCK");
    
    constexpr float x2 = SERIAL_CLOCK / (MULTIPLEX * MAX_REFRESH * (1 << std::max(PWM_bits, (int) seg_bits)));
    static_assert(x2 >= 1.0, "SERIAL_CLOCK too low");
    constexpr float GCLK = x2 * 125000000.0 / (SERIAL_CLOCK * 2.0);
    static_assert(GCLK >= 1.0, "Unabled to configure PIO for MAX_REFRESH");

    // DAT SM
    pio0->sm[0].clkdiv = 0;
    pio0->sm[0].execctrl = (1 << 17) | (2 << 12);
    pio0->sm[0].shiftctrl = (1 << PIO_SM0_SHIFTCTRL_AUTOPULL_LSB) | (16 << 25) | (1 << 19);
    pio0->sm[0].pinctrl = (1 << PIO_SM0_PINCTRL_OUT_COUNT_LSB) | 8;
    pio0->sm[0].instr = pio_encode_jmp(0);
    hw_set_bits(&pio0->ctrl, 1 << PIO_CTRL_SM_ENABLE_LSB);
    pio_sm_claim(pio0, 0);

    // CLK/LAT SM
    pio0->sm[1].clkdiv = ((uint32_t) floor(CLK) << 16) | ((uint32_t) round((CLK - floor(CLK)) * 255.0) << 8);
    pio0->sm[1].execctrl = (13 << 12);
    pio0->sm[1].shiftctrl = 0;
    pio0->sm[1].pinctrl = (2 << PIO_SM0_PINCTRL_SIDESET_COUNT_LSB) | (14 << PIO_SM0_PINCTRL_SIDESET_BASE_LSB);
    pio0->sm[1].instr = pio_encode_jmp(3);
    hw_set_bits(&pio0->ctrl, 2 << PIO_CTRL_SM_ENABLE_LSB);
    pio_sm_claim(pio0, 1);

    // GCLK SM
    pio0->sm[2].clkdiv = ((uint32_t) floor(GCLK) << 16) | ((uint32_t) round((GCLK - floor(GCLK)) * 255.0) << 8);
    pio0->sm[2].execctrl = (18 << 12);
    pio0->sm[2].shiftctrl = 0;
    pio0->sm[2].pinctrl = (1 << PIO_SM0_PINCTRL_SIDESET_COUNT_LSB) | (22 << PIO_SM0_PINCTRL_SIDESET_BASE_LSB);
    pio0->sm[2].instr = pio_encode_jmp(14);
    hw_set_bits(&pio0->ctrl, 4 << PIO_CTRL_SM_ENABLE_LSB);
    pio_sm_claim(pio0, 2);
    
    // DMA
    bus_ctrl_hw->priority = (1 << 12) | (1 << 8);
    dma_chan = dma_claim_unused_channel(true);
    
    // DAT DMA
    dma_channel_config c = dma_channel_get_default_config(dma_chan);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_16);
    channel_config_set_read_increment(&c, true);
    channel_config_set_dreq(&c, DREQ_PIO0_TX0);
    dma_channel_configure(dma_chan, &c, &pio0_hw->txf[0], NULL, COLUMNS / 16 * 3, false);
    
    // Boot procedure
    uint16_t cfg1[3 * COLUMNS / 16];
    uint16_t cfg2[3 * COLUMNS / 16];
    
    for (uint8_t i = 0; i < COLUMNS / 16; i++) {
        cfg1[(i * 3) + 0] = 0x15 | (((uint16_t) round(BLUE_GAIN * 510) & 0x3) << 14);
        cfg1[(i * 3) + 1] = ((uint16_t) round(GREEN_GAIN * 510) << (23 - 16)) | ((uint16_t) round(BLUE_GAIN * 510) >> 2);
        cfg1[(i * 3) + 2] = (0x9 << (44 - 32)) | (0x7 << (41 - 32)) | ((uint16_t) round(RED_GAIN * 510) << (32 - 32));
        cfg2[(i * 3) + 0] = MULTIPLEX | (1 << 15);
        cfg2[(i * 3) + 1] = (1 << (19 - 16));
        cfg2[(i * 3) + 2] = (0x6 << (44 - 32));
    }
    
    send_cmd(15);
    dma_channel_set_read_addr(dma_chan, cfg1, true);
    pio_sm_put_blocking(0, 1, (COLUMNS / 16 * 3) - 5);
    pio_sm_put_blocking(0, 1, 5);
    send_cmd(15);
    dma_channel_set_read_addr(dma_chan, cfg2, true);
    pio_sm_put_blocking(0, 1, (COLUMNS / 16 * 3) - 5);
    pio_sm_put_blocking(0, 1, 5);
    
    dma_channel_set_irq0_enabled(dma_chan, true); 
    
    start_gclk(seg_bits);                                                       // Kick off hardware (GCLK)
}

// Keep this short and sweet!
void __not_in_flash_func(matrix_dma_isr)() { 
    static uint16_t counter = 0;
       
    if (dma_channel_get_irq0_status(dma_chan)) {
        if (++counter >= (MULTIPLEX * 16)) {                                    // Fire rate: MULTIPLEX * 16 * FPS
            isFinished = true;
            counter = 0;
        }
        else
            start_clk(counter, lat_cmd);
        dma_hw->ints0 = 1 << dma_chan;
    }
}

void __not_in_flash_func(matrix_gclk_task)() {
    static uint32_t rows = 0;
    extern volatile bool vsync;
    uint64_t time = time_us_64();                                               // Start a timer with 1uS delay
    
    if (pio0->sm[2].instr == 14) {
        if (++rows >= MULTIPLEX) {
            rows = 0;
            if (vsync) {
                bank = (bank + 1) % 3;
                vsync = false;
                start_clk(0, lat_cmd);                                          // Kick off hardware (CLK)
            }
            if (isFinished) {
                send_cmd(3);                                                    // VSYNC Procedure
                SetRow(rows);
                time = time_us_64();                                            // Restart timer with 3uS delay
                while((time_us_64() - time) < (uint64_t) std::max(BLANK_TIME, 3));  // Check if timer has expired
                return;
            }
        }
        SetRow(rows);

        while((time_us_64() - time) < BLANK_TIME);                              // Check if timer has expired
        start_gclk(seg_bits);                                                   // Kick off hardware (GCLK)
    }
}

void start_clk(uint16_t counter, uint8_t cmd) {
    dma_channel_set_read_addr(dma_chan, buf[bank][counter / 16][counter % 16], true);
    pio_sm_put_blocking(0, 1, (COLUMNS / 16 * 3) - cmd);
    pio_sm_put_blocking(0, 1, cmd);
}

void start_gclk(uint8_t bits) {
    pio_sm_put_blocking(0, 2, (1 << seg_bits) + 1);
}

void send_cmd(uint8_t cmd) {
    pio_sm_put_blocking(0, 1, 1);
    pio_sm_put_blocking(0, 1, cmd);
    while (pio0->sm[1].instr != 3); 
}

