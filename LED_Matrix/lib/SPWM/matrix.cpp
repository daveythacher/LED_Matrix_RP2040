/* 
 * File:   matrix.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#include <stdint.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/pio.h"
#include "hardware/gpio.h"
#include "hardware/dma.h"
#include "SPWM/config.h"
#include "Multiplex/Multiplex.h"

test2 buf[2];
volatile uint8_t bank = 0;
static volatile bool stop = false;
static int dma_chan;
static Multiplex *m;

void matrix_dma_isr();
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
    const uint16_t instructions[] = { 
        (uint16_t) (pio_encode_out(pio_pins, 8) | pio_encode_sideset(1, 0)),    // PMP Program
        (uint16_t) (pio_encode_nop() | pio_encode_sideset(1, 1)),
        (uint16_t) (pio_encode_pull(false, true)  | pio_encode_sideset(1, 1)),  // OE Program
        (uint16_t) (pio_encode_mov(pio_x, pio_osr) | pio_encode_sideset(1, 0)), 
        (uint16_t) (pio_encode_jmp_x_dec(8)),
        (uint16_t) (pio_encode_nop())
    };
    static const struct pio_program pio_programs = {
        .instructions = instructions,
        .length = count_of(instructions),
        .origin = -1,
    };
    pio_add_program(pio0, &pio_programs);
    
    // Verify FPS and Refresh
    constexpr float x3 = MAX_REFRESH / (FPS * (1 << PWM_bits)) * 1.0;
    static_assert(x3 >= 1.0);
    
    // Verify Serial Clock
    constexpr float x2 = SERIAL_CLOCK / (MULTIPLEX * COLUMNS * MAX_REFRESH);
    assert(x2 >= 1.0);
    constexpr float x = x2 * 125000000.0 / (SERIAL_CLOCK * 2.0);
    static_assert(x >= 1.0);

    // PMP
    pio_sm_set_consecutive_pindirs(pio0, 0, 0, 9, true);
    pio0->sm[0].clkdiv = ((uint32_t) floor(x) << 16) | ((uint32_t) round((x - floor(x)) * 255.0) << 8);
    pio0->sm[0].pinctrl = (1 << PIO_SM0_PINCTRL_SIDESET_COUNT_LSB) | (8 << PIO_SM0_PINCTRL_OUT_COUNT_LSB) | (8 << PIO_SM0_PINCTRL_SIDESET_BASE_LSB);
    pio0->sm[0].shiftctrl = (1 << PIO_SM0_SHIFTCTRL_AUTOPULL_LSB) | (8 << 25) | (1 << 19);
    pio0->sm[0].execctrl = (1 << 17) | (0x1 << 12);
    hw_set_bits(&pio0->ctrl, 1 << PIO_CTRL_SM_ENABLE_LSB);
    pio_sm_claim(pio0, 0);
    
    // OE
    pio0->sm[1].clkdiv = (8 << 16) | (0 << 8);                                  // Note: 125MHz / 8 = 15.625MHz - 8 + (0/256)
    pio0->sm[1].pinctrl = (1 << PIO_SM1_PINCTRL_SIDESET_COUNT_LSB) | (10 << PIO_SM1_PINCTRL_SIDESET_BASE_LSB);
    pio0->sm[1].execctrl = (1 << 17) | (0x5 << 12) | (0x2 << 7);
    pio0->sm[1].instr = pio_encode_jmp(0x2);
    hw_set_bits(&pio0->ctrl, 2 << PIO_CTRL_SM_ENABLE_LSB);
    pio_sm_claim(pio0, 1);
    
    // DMA
    dma_chan = dma_claim_unused_channel(true);
    dma_channel_config c = dma_channel_get_default_config(dma_chan);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
    channel_config_set_read_increment(&c, true);
    channel_config_set_dreq(&c, DREQ_PIO0_TX0);
    dma_channel_configure(dma_chan, &c, &pio0_hw->txf[0], NULL, COLUMNS, false);
    dma_channel_set_irq0_enabled(dma_chan, true); 
    send_line(buf[1][0][0]);
}

static void __not_in_flash_func(send_latch)() {
    gpio_set_mask(1 << 9);
    __asm__ __volatile__ ("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
    gpio_clr_mask(1 << 9);
}

void __not_in_flash_func(send_line)(uint8_t *line) {
    dma_hw->ints0 = 1 << dma_chan;
    dma_channel_set_read_addr(dma_chan, line, true);
    pio_sm_put(pio0, 1, COLUMNS * 2 / POWER_DIVISOR);                           // Start a timer for OE using PIO
}

void __not_in_flash_func(matrix_dma_isr)() {
    static uint32_t rows = 0;
    static uint32_t counter = 0;
    
    uint32_t time = time_us_32();                                               // Start a timer with 1uS delay using PIO
    m->SetRow(rows);
    
    if (++rows >= MULTIPLEX) {
        rows = 0;
        if (++counter >= (1 << PWM_bits))
            counter = 0;
    }  
    
    send_latch();
    while((time_us_32() - time) < 1);                                           // Check if timer has expired
    
    // Kick off hardware to get ISR ticks
    send_line(buf[(bank + 1) % 2][rows][counter]);
}

