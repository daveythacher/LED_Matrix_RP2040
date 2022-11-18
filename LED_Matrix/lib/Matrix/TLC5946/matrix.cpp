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
#include "Matrix/TLC5946/memory_format.h"
#include "Multiplex/Multiplex.h"

// NOTE: This uses custom hardware board!
//  It is not compatible with the Interface boards!

// TODO: MODE, XHALF, XERR

test2 buf[3];
static volatile uint8_t bank = 0;
static int dma_chan[3];
static volatile bool isFinished[2] = { false, false };

static void start_clk(uint16_t counter);
static void start_gclk();

void matrix_start() {
    // Init Matrix hardware
    // IO
    for (int i = 0; i < 13; i++) {
        gpio_init(i + 8);
        gpio_set_dir(i + 8, GPIO_OUT);
    }
    for (int i = 0; i < 7; i++)
        gpio_set_function(i + 8, GPIO_FUNC_PIO0);
    gpio_init(20);
    gpio_set_dir(20, GPIO_OUT);
    gpio_init(21);
    gpio_set_dir(21, GPIO_OUT);
    gpio_set_function(21, GPIO_FUNC_PIO1);
    gpio_clr_mask(0x3FFF00);
    
    /*
        1 - CLK/LAT
        3 - DAT
        1 - GCLK
    */
    
    // PIO
    const uint16_t instructions[] = {
        (uint16_t) (pio_encode_out(pio_pins, 1)),                               // DAT Program (125 MHz Clock)
        (uint16_t) (pio_encode_wait_gpio(true, 14)),
        (uint16_t) (pio_encode_wait_gpio(false, 14)),
        (uint16_t) (pio_encode_pull(false, true) | pio_encode_sideset(2, 0)),   // CLK Program (Cannot exceed 25MHz Clock)
        (uint16_t) (pio_encode_mov(pio_x, pio_osr) | pio_encode_sideset(2, 0)),
        (uint16_t) (pio_encode_nop() | pio_encode_sideset(2, 0)),
        (uint16_t) (pio_encode_jmp_x_dec(5) | pio_encode_sideset(2, 1)),
        (uint16_t) (pio_encode_nop() | pio_encode_sideset(2, 0)),
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
    pio_sm_set_consecutive_pindirs(pio0, 3, 14, 2, true);
    pio_sm_set_consecutive_pindirs(pio1, 0, 21, 1, true);
    
    // Verify Serial Clock
    static_assert(SERIAL_CLOCK <= 25000000.0, "Code limits clock for TLC5946 to 25MHz");
    
    constexpr float x = SERIAL_CLOCK / (MULTIPLEX * COLUMNS * MAX_REFRESH * 12);
    static_assert(x >= 1.0, "SERIAL_CLOCK too low");
    constexpr float CLK = x * 125000000.0 / (SERIAL_CLOCK * 2.0);
    static_assert(CLK >= 1.0, "Unabled to configure PIO for SERIAL_CLOCK");
    
    constexpr float x2 = SERIAL_CLOCK / (MULTIPLEX * MAX_REFRESH * (1 << PWM_bits));
    static_assert(x2 >= 1.0, "SERIAL_CLOCK too low");
    constexpr float GCLK = x2 * 125000000.0 / (SERIAL_CLOCK * 2.0);
    static_assert(GCLK >= 1.0, "Unabled to configure PIO for MAX_REFRESH");

    // DAT SM
    pio0->sm[0].clkdiv = 0;
    pio0->sm[0].execctrl = (1 << 17) | (2 << 12);
    pio0->sm[0].shiftctrl = (1 << PIO_SM0_SHIFTCTRL_AUTOPULL_LSB) | (12 << 25) | (1 << 19);
    pio0->sm[0].pinctrl = (1 << PIO_SM0_PINCTRL_OUT_COUNT_LSB) | 8;
    pio0->sm[0].instr = pio_encode_jmp(0);
    hw_set_bits(&pio0->ctrl, 1 << PIO_CTRL_SM_ENABLE_LSB);
    pio_sm_claim(pio0, 0);
    
    pio0->sm[1].clkdiv = 0;
    pio0->sm[1].execctrl = (1 << 17) | (2 << 12);
    pio0->sm[1].shiftctrl = (1 << PIO_SM0_SHIFTCTRL_AUTOPULL_LSB) | (12 << 25) | (1 << 19);
    pio0->sm[1].pinctrl = (1 << PIO_SM0_PINCTRL_OUT_COUNT_LSB) | 8;
    pio0->sm[1].instr = pio_encode_jmp(0);
    hw_set_bits(&pio0->ctrl, 2 << PIO_CTRL_SM_ENABLE_LSB);
    pio_sm_claim(pio0, 1);
    
    pio0->sm[2].clkdiv = 0;
    pio0->sm[2].execctrl = (1 << 17) | (2 << 12);
    pio0->sm[2].shiftctrl = (1 << PIO_SM0_SHIFTCTRL_AUTOPULL_LSB) | (12 << 25) | (1 << 19);
    pio0->sm[2].pinctrl = (1 << PIO_SM0_PINCTRL_OUT_COUNT_LSB) | 8;
    pio0->sm[2].instr = pio_encode_jmp(0);
    hw_set_bits(&pio0->ctrl, 4 << PIO_CTRL_SM_ENABLE_LSB);
    pio_sm_claim(pio0, 2);

    // CLK/LAT SM
    pio0->sm[3].clkdiv = ((uint32_t) floor(CLK) << 16) | ((uint32_t) round((CLK - floor(CLK)) * 255.0) << 8);
    pio0->sm[3].execctrl = (7 << 12);  // Wrap
    pio0->sm[3].shiftctrl = 0;
    pio0->sm[3].pinctrl = (2 << PIO_SM0_PINCTRL_SIDESET_COUNT_LSB) | (14 << PIO_SM0_PINCTRL_SIDESET_BASE_LSB);
    pio0->sm[3].instr = pio_encode_jmp(3);
    hw_set_bits(&pio0->ctrl, 8 << PIO_CTRL_SM_ENABLE_LSB);
    pio_sm_claim(pio0, 3);

    // GCLK SM
    pio1->sm[0].clkdiv = ((uint32_t) floor(GCLK) << 16) | ((uint32_t) round((GCLK - floor(GCLK)) * 255.0) << 8);
    pio1->sm[0].execctrl = (12 << 12);  // Wrap
    pio1->sm[0].shiftctrl = 0;
    pio1->sm[0].pinctrl = (1 << PIO_SM0_PINCTRL_SIDESET_COUNT_LSB) | (21 << PIO_SM0_PINCTRL_SIDESET_BASE_LSB);
    pio1->sm[0].instr = pio_encode_jmp(8);
    hw_set_bits(&pio1->ctrl, 1 << PIO_CTRL_SM_ENABLE_LSB);
    pio_sm_claim(pio1, 0);
    
    // DMA
    bus_ctrl_hw->priority = (1 << 12) | (1 << 8);
    
    // DAT DMA
    dma_channel_config c;
    
    dma_chan[0] = dma_claim_unused_channel(true);
    c = dma_channel_get_default_config(dma_chan[0]);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_16);
    channel_config_set_read_increment(&c, true);
    channel_config_set_dreq(&c, DREQ_PIO0_TX0);
    dma_channel_configure(dma_chan[0], &c, &pio0_hw->txf[0], NULL, COLUMNS, false);
    
    dma_chan[1] = dma_claim_unused_channel(true);
    c = dma_channel_get_default_config(dma_chan[1]);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_16);
    channel_config_set_read_increment(&c, true);
    channel_config_set_dreq(&c, DREQ_PIO0_TX1);
    dma_channel_configure(dma_chan[1], &c, &pio0_hw->txf[1], NULL, COLUMNS, false);
    
    dma_chan[2] = dma_claim_unused_channel(true);
    c = dma_channel_get_default_config(dma_chan[2]);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_16);
    channel_config_set_read_increment(&c, true);
    channel_config_set_dreq(&c, DREQ_PIO0_TX2);
    dma_channel_configure(dma_chan[2], &c, &pio0_hw->txf[2], NULL, COLUMNS, false);
    
    // TODO: Boot procedure
    gpio_set_mask(1 << 20);                                                     // Disable display and reset GCLK counter
    
    dma_channel_set_irq0_enabled(dma_chan[2], true); 
    start_clk(0);
}

// Keep this short and sweet!
void __not_in_flash_func(matrix_dma_isr)() { 
    static uint16_t counter = 0;
       
    if (dma_channel_get_irq0_status(dma_chan[2])) {
        if (++counter >= MULTIPLEX)                                             // Fire rate: MULTIPLEX * MAX_REFRESH
            counter = 0;
            
        gpio_set_mask(1 << 15);                                                 // Send out LAT pulse (5/125MHz = 40nS 1/80nS = 12.5MHz)
        asm("nop; nop; nop; nop; nop;");
        gpio_clr_mask(1 << 15);
        
        isFinished[1] = true;                                                   // Sync point (Blocking could cause problem, due to ISR)
        while (!isFinished[0]);
        isFinished[0] = false;
        
        start_clk(counter);
        dma_hw->ints0 = 1 << dma_chan[2];
        start_gclk();                                                           // Kick off hardware (GCLK)
    }
}

void __not_in_flash_func(matrix_gclk_task)() {
    static uint32_t rows = 0;
    extern volatile bool vsync;
    uint64_t time;
    
    if (pio1->sm[0].instr == 8) {                                               // Check if blocked on FIFO
        gpio_set_mask(1 << 20);                                                 // Disable display and reset GCLK counter
        time = time_us_64();                                                    // Start a timer with 1uS delay
        
        if (++rows >= MULTIPLEX) {
            rows = 0;
            if (vsync) {
                bank = (bank + 1) % 3;
                vsync = false;
            }
        }
        
        SetRow(rows);
        while((time_us_64() - time) < BLANK_TIME);                              // Check if timer has expired
        
        isFinished[0] = true;                                                   // Sync point (Blocking does not cause issue)
        while (!isFinished[1]);
        gpio_clr_mask(1 << 20);                                                 // Enable display and reset GCLK counter
        isFinished[1] = false;
    }
}

void __not_in_flash_func(start_clk)(uint16_t counter) {
    dma_channel_set_read_addr(dma_chan[0], buf[bank][counter][0], true);
    dma_channel_set_read_addr(dma_chan[1], buf[bank][counter][1], true);
    dma_channel_set_read_addr(dma_chan[2], buf[bank][counter][2], true);
    pio_sm_put_blocking(pio0, 3, COLUMNS);
}

void __not_in_flash_func(start_gclk)() {
    pio_sm_put_blocking(pio1, 0, 1 << PWM_bits);
}

