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
#include "hardware/irq.h"
#include "BCM/config.h"
#include "Multiplex/Multiplex.h"

test2 buf[2];
volatile uint8_t bank = 0;
static volatile bool stop = false;
static int dma_chan;
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
        pio_encode_out(pio_pins, 8) | pio_encode_sideset(1, 0) , pio_encode_nop() | pio_encode_sideset(1, 1),                   // PMP Program
        pio_encode_pull(false, true), pio_encode_mov(pio_x, pio_osr), pio_encode_jmp_x_dec(4), pio_encode_irq_wait(false, 0)    // Delay Program
    };
    for (uint i = 0; i < count_of(instructions); i++)
        pio0->instr_mem[i] = instructions[i];

    // PMP
    pio_sm_set_consecutive_pindirs(pio0, 0, 0, 9, true);
    pio0->sm[0].clkdiv = (8 << 16) | (0 << 8);          // Note: 125MHz / 8 = 15.625MHz - 8 + (0/256)
    pio0->sm[0].pinctrl = (1 << PIO_SM0_PINCTRL_SIDESET_COUNT_LSB) | (8 << PIO_SM0_PINCTRL_OUT_COUNT_LSB) | (8 << PIO_SM0_PINCTRL_SIDESET_BASE_LSB);
    pio0->sm[0].shiftctrl = (1 << PIO_SM0_SHIFTCTRL_AUTOPULL_LSB) | (8 << 25) | (1 << 19);
    pio0->sm[0].execctrl = (1 << 17) | (0x1 << 12);
    hw_set_bits(&pio0->ctrl, 1 << PIO_CTRL_SM_ENABLE_LSB);
    
    // Delay
    pio0->sm[1].clkdiv = (1 << 16) | (0 << 8);          // Note: 125MHz / 1 = 125MHz - 1 + (0/256)
    pio0->sm[1].execctrl = (0x5 << 12) | (0x2 << 7);
    pio0->irq = 1 << 0;
    pio0->sm[1].instr = pio_encode_jmp(0x2);
    hw_set_bits(&pio0->ctrl, 2 << PIO_CTRL_SM_ENABLE_LSB);
    
    // OE
    pio0->sm[2].clkdiv = (8 << 16) | (0 << 8);          // Note: 125MHz / 8 = 15.625MHz - 8 + (0/256)
    pio0->sm[2].execctrl = (0x5 << 12) | (0x2 << 7);
    pio0->irq = 1 << 1;
    pio0->sm[2].instr = pio_encode_jmp(0x2);
    hw_set_bits(&pio0->ctrl, 4 << PIO_CTRL_SM_ENABLE_LSB);
    // TODO: Configure PIN 10 (OE)
    // TODO: Add IO operations to PIO instructions
    
    // DMA
    dma_chan = dma_claim_unused_channel(true);
    dma_channel_config c = dma_channel_get_default_config(dma_chan);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
    channel_config_set_read_increment(&c, true);
    channel_config_set_dreq(&c, DREQ_PIO0_TX0);
    dma_channel_configure(dma_chan, &c, &pio0_hw->txf[0], NULL, COLUMNS, false);
    dma_channel_set_irq0_enabled(dma_chan, true);
    irq_set_exclusive_handler(DMA_IRQ_0, isr);
    irq_set_priority(DMA_IRQ_0, 0);
    irq_set_enabled(DMA_IRQ_0, true);    
    send_line(buf[1][0][0]);
    
    extern void work();
    multicore_launch_core1(work);
}

static void __not_in_flash_func(send_latch)() {
    gpio_set_mask(1 << 9);
    __asm__ __volatile__ ("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
    gpio_clr_mask(1 << 9);
}

void __not_in_flash_func(send_line)(uint8_t *line) {
    dma_hw->ints0 = 1 << dma_chan;
    dma_channel_set_read_addr(dma_chan, line, true);
    pio_sm_put(pio0, 2, COLUMNS * 2 / POWER_DIVISOR);   // Start a timer for OE using PIO
}

void __not_in_flash_func(isr)() {
    static uint32_t rows = 0;
    static uint32_t counter = 0;
    static uint32_t bits = 0;
    
    if (counter == 0) {
        pio_sm_put(pio0, 1, 1000 / 8);                  // Start a timer with 1uS delay using PIO
        m->SetRow(rows);
    }
     
    if (++counter >= (uint32_t) (1 << bits)) {          // Note: Could optimize this to use less interrupts
        counter = 0;
        if (++bits >= PWM_bits) {
            bits = 0;
            if (++rows >= MULTIPLEX)
                rows = 0;
        }
    }
    
    send_latch();
    if (counter == 1) {
        while(!pio_interrupt_get(pio0, 0));             // Check if timer has expired
        pio0->irq = 1 << 1;                             // Release timer
    }
    
    // Kick off hardware to get ISR ticks
    send_line(buf[(bank + 1) % 2][rows][bits]);
}

