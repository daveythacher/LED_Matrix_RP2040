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
#include "hardware/irq.h"
#include "hardware/timer.h"
#include "MBI5153/config.h"
#include "Multiplex/Multiplex.h"

test2 buf[2];
volatile uint8_t bank = 0;
static volatile bool stop = false;
static int dma_chan[6];
static Multiplex *m;
static uint32_t rows = 1;
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
    for (int i = 4; i < 11; i++)
        gpio_set_function(i, GPIO_FUNC_PIO1);
    gpio_clr_mask(0x7FFF);
    m = Multiplex::getMultiplexer(MULTIPLEX_NUM);
    
    // PIO                                                                  // Note: There is a lot of loser timing issues.
    const uint16_t instructions[] = { 
        (uint16_t) (pio_encode_out(pio_pins, 1)),                           // PMP Program
        (uint16_t) (pio_encode_wait_gpio(true, 7)),
        (uint16_t) (pio_encode_jmp(0)),
        (uint16_t) (pio_encode_pull(false, true)),                          // CLK/LAT Program
        (uint16_t) (pio_encode_mov(pio_x, pio_osr) | pio_encode_sideset(2, 0)),
        (uint16_t) (pio_encode_pull(false, true)),
        (uint16_t) (pio_encode_mov(pio_y, pio_osr)),
        (uint16_t) (pio_encode_nop() | pio_encode_sideset(3, 3)),
        (uint16_t) (pio_encode_nop() | pio_encode_sideset(3, 3)),
        (uint16_t) (pio_encode_nop() | pio_encode_sideset(3, 0)),
        (uint16_t) (pio_encode_jmp_x_dec(7) | pio_encode_sideset(3, 0)),
        (uint16_t) (pio_encode_nop() | pio_encode_sideset(3, 4)),
        (uint16_t) (pio_encode_nop() | pio_encode_sideset(3, 4)),
        (uint16_t) (pio_encode_nop() | pio_encode_sideset(3, 7)),
        (uint16_t) (pio_encode_jmp_y_dec(11) | pio_encode_sideset(3, 7)),
        (uint16_t) (pio_encode_irq_set(false, 0)),
        (uint16_t) (pio_encode_jmp(3)),
        (uint16_t) (pio_encode_pull(false, true)),                          // GCLK Program
        (uint16_t) (pio_encode_mov(pio_x, pio_osr)),
        (uint16_t) (pio_encode_nop() | pio_encode_sideset(1, 1)),
        (uint16_t) (pio_encode_jmp_x_dec(19) | pio_encode_sideset(1, 0)),
        (uint16_t) (pio_encode_irq_set(false, 1)),
        (uint16_t) (pio_encode_irq_wait(false, 1)),
        (uint16_t) (pio_encode_jmp(17))
    };
    static const struct pio_program pio_programs = {
        .instructions = instructions,
        .length = count_of(instructions),
        .origin = 0,
    };
    pio_add_program(pio0, &pio_programs);
    pio_add_program(pio1, &pio_programs);
    pio_sm_set_consecutive_pindirs(pio0, 0, 0, 1, true);
    pio_sm_set_consecutive_pindirs(pio0, 1, 1, 1, true);
    pio_sm_set_consecutive_pindirs(pio0, 2, 2, 1, true);
    pio_sm_set_consecutive_pindirs(pio0, 3, 3, 1, true);
    pio_sm_set_consecutive_pindirs(pio1, 0, 4, 1, true);
    pio_sm_set_consecutive_pindirs(pio1, 1, 5, 1, true);
    pio_sm_set_consecutive_pindirs(pio1, 2, 7, 3, true);
    pio_sm_set_consecutive_pindirs(pio1, 3, 10, 1, true);
    pio_set_irq1_source_enabled(pio1, pis_interrupt0, true);
    pio_set_irq0_source_enabled(pio1, pis_interrupt1, true);

/*
    6 PIOs for shifters
    1 PIO for shift clock / latch
    1 PIO for GCLK
*/

    // Verify Serial Clock
    constexpr float x2 = 125000000.0 / (MULTIPLEX * (1 << PWM_bits) * MAX_REFRESH * 2.0);
    static_assert(SERIAL_CLOCK / (MULTIPLEX * 1.0 * (1 << PWM_bits) * MAX_REFRESH) >= 1.0);
    static_assert(x2 >= 1.0);
    constexpr float x = 125000000.0 / (SERIAL_CLOCK * 0.8 * 4.0);
    static_assert((SERIAL_CLOCK * 0.8) / (FPS * COLUMNS * 16.0 * MULTIPLEX) >= 1.0);
    static_assert(x >= 1.0);

    // Parallel shifter bus
    pio0->sm[0].clkdiv = ((uint32_t) floor(x) << 16) | ((uint32_t) round((x - floor(x)) * 255.0) << 8);
    pio0->sm[0].pinctrl = (1 << PIO_SM0_PINCTRL_OUT_COUNT_LSB) | 0x0;
    pio0->sm[0].shiftctrl = (1 << PIO_SM0_SHIFTCTRL_AUTOPULL_LSB) | (16 << 25) | (1 << 19);
    pio0->sm[0].execctrl = (1 << 17) | (0x1F << 12);
    pio0->sm[0].instr = pio_encode_jmp(0);
    hw_set_bits(&pio0->ctrl, 1 << PIO_CTRL_SM_ENABLE_LSB);
    pio0->sm[1].clkdiv = ((uint32_t) floor(x) << 16) | ((uint32_t) round((x - floor(x)) * 255.0) << 8);
    pio0->sm[1].pinctrl = (1 << PIO_SM0_PINCTRL_OUT_COUNT_LSB) | 0x1;
    pio0->sm[1].shiftctrl = (1 << PIO_SM0_SHIFTCTRL_AUTOPULL_LSB) | (16 << 25) | (1 << 19);
    pio0->sm[1].execctrl = (1 << 17) | (0x1F << 12);
    pio0->sm[1].instr = pio_encode_jmp(0);
    hw_set_bits(&pio0->ctrl, 2 << PIO_CTRL_SM_ENABLE_LSB);
    pio0->sm[2].clkdiv = ((uint32_t) floor(x) << 16) | ((uint32_t) round((x - floor(x)) * 255.0) << 8);
    pio0->sm[2].pinctrl = (1 << PIO_SM0_PINCTRL_OUT_COUNT_LSB) | 0x2;
    pio0->sm[2].shiftctrl = (1 << PIO_SM0_SHIFTCTRL_AUTOPULL_LSB) | (16 << 25) | (1 << 19);
    pio0->sm[2].execctrl = (1 << 17) | (0x1F << 12);
    pio0->sm[2].instr = pio_encode_jmp(0);
    hw_set_bits(&pio0->ctrl, 4 << PIO_CTRL_SM_ENABLE_LSB);
    pio0->sm[3].clkdiv = ((uint32_t) floor(x) << 16) | ((uint32_t) round((x - floor(x)) * 255.0) << 8);
    pio0->sm[3].pinctrl = (1 << PIO_SM0_PINCTRL_OUT_COUNT_LSB) | 0x3;
    pio0->sm[3].shiftctrl = (1 << PIO_SM0_SHIFTCTRL_AUTOPULL_LSB) | (16 << 25) | (1 << 19);
    pio0->sm[3].execctrl = (1 << 17) | (0x1F << 12);
    pio0->sm[3].instr = pio_encode_jmp(0);
    hw_set_bits(&pio0->ctrl, 8 << PIO_CTRL_SM_ENABLE_LSB);
    pio1->sm[0].clkdiv = ((uint32_t) floor(x) << 16) | ((uint32_t) round((x - floor(x)) * 255.0) << 8);
    pio1->sm[0].pinctrl = (1 << PIO_SM0_PINCTRL_OUT_COUNT_LSB) | 0x4;
    pio1->sm[0].shiftctrl = (1 << PIO_SM0_SHIFTCTRL_AUTOPULL_LSB) | (16 << 25) | (1 << 19);
    pio1->sm[0].execctrl = (1 << 17) | (0x1F << 12);
    pio1->sm[0].instr = pio_encode_jmp(0);
    hw_set_bits(&pio1->ctrl, 1 << PIO_CTRL_SM_ENABLE_LSB);
    pio1->sm[1].clkdiv = ((uint32_t) floor(x) << 16) | ((uint32_t) round((x - floor(x)) * 255.0) << 8);
    pio1->sm[1].pinctrl = (1 << PIO_SM0_PINCTRL_OUT_COUNT_LSB) | 0x5;
    pio1->sm[1].shiftctrl = (1 << PIO_SM0_SHIFTCTRL_AUTOPULL_LSB) | (16 << 25) | (1 << 19);
    pio1->sm[1].execctrl = (1 << 17) | (0x1F << 12);
    pio1->sm[1].instr = pio_encode_jmp(0);
    hw_set_bits(&pio1->ctrl, 2 << PIO_CTRL_SM_ENABLE_LSB);
    
    // CLK and LAT
    pio1->sm[2].clkdiv = ((uint32_t) floor(x) << 16) | ((uint32_t) round((x - floor(x)) * 255.0) << 8);
    pio1->sm[2].pinctrl = (3 << PIO_SM0_PINCTRL_SIDESET_COUNT_LSB) | (7 << PIO_SM0_PINCTRL_SIDESET_BASE_LSB);
    pio1->sm[2].shiftctrl = 0;
    pio1->sm[2].execctrl = (1 << 17) | (0x1F << 12);
    pio1->sm[2].instr = pio_encode_jmp(3);
    hw_set_bits(&pio1->ctrl, 4 << PIO_CTRL_SM_ENABLE_LSB);
    
    // GCLK
    pio1->sm[3].clkdiv = ((uint32_t) floor(x2) << 16) | ((uint32_t) round((x2 - floor(x2)) * 255.0) << 8);
    pio1->sm[3].pinctrl = (1 << PIO_SM0_PINCTRL_SIDESET_COUNT_LSB) | (10 << PIO_SM0_PINCTRL_SIDESET_BASE_LSB);
    pio1->sm[3].shiftctrl = 0;
    pio1->sm[3].execctrl = (1 << 17) | (0x1F << 12);
    pio1->sm[3].instr = pio_encode_jmp(17);
    hw_set_bits(&pio1->ctrl, 8 << PIO_CTRL_SM_ENABLE_LSB);
    
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

// Note: This is a total loser hack.
void __not_in_flash_func(send_cmd)(uint8_t cmd) {
    pio_sm_put(pio1, 2, COLUMNS - cmd + 1);
    pio_sm_put(pio1, 2, cmd - 1);
}

// Note: This is the lower priority ISR which can be preempted by the other.
void __not_in_flash_func(matrix_pio_isr0)() {    
    static uint8_t row = 0;
    static uint8_t counter = 0;
    
    if (pio_interrupt_get(pio1, 0)) {
        if (++counter >= (COLUMNS / 16)) {
            counter = 1;
            if (++row >= MULTIPLEX) {
                uint64_t time = time_us_64();
                row = 1; 
                while ((time_us_64() - time) < 3);                          // Wait 50 GCLKs
                stop = true;
                while(stop);                                                // Note: Loser timing hack
                gpio_init(10);
                gpio_set_dir(10, GPIO_OUT);
                gpio_set_mask(1 << 10);
                pio_interrupt_clear(pio1, 0);
                send_cmd(2);                                                // Send VSYNC CMD
                while(!pio_interrupt_get(pio1, 0));
                gpio_init(8);
                gpio_set_dir(8, GPIO_OUT);
                gpio_set_mask(1 << 8);
                m->SetRow(rows);

                // Dead time
                time = time_us_64();
                while((time_us_64() - time) < 1);
                gpio_clr_mask(1 << 10);
                gpio_clr_mask(1 << 8);
                while((time_us_64() - time) < 3);    
                    
                // Kick off hardware to get ISR ticks (GCLK)
                gpio_set_function(10, GPIO_FUNC_PIO1);
                gpio_set_function(8, GPIO_FUNC_PIO1);
                pio_sm_put(pio1, 3, 512);
            }
        }
        
        // Note: This is slow! Adds about 1uS
        for (int i = 0; i < 6; i++)
            dma_channel_set_read_addr(dma_chan[i], (uint8_t *) &buf[bank][i][row - 1][counter - 1], true);
        send_cmd(1);
        pio_interrupt_clear(pio1, 0);
    }
}

// Note: This is the higher priority ISR which can be preempt the other.
void __not_in_flash_func(matrix_pio_isr1)() {    
    if (pio_interrupt_get(pio1, 1)) {  
        gpio_init(10);
        gpio_set_dir(10, GPIO_OUT);
        gpio_set_mask(1 << 10);   
        uint64_t time = time_us_64();
        
        if (stop) {                                                         // Bail if VSYNC is pending
            stop = false;
            return pio_interrupt_clear(pio1, 1);
        }
        
        m->SetRow(rows);
        if (++rows >= MULTIPLEX)
            rows = 0;
            
        // Support for anti-ghosting/pre-charge FET
        while((time_us_64() - time) < (uint32_t) std::min(1, BLANK_TIME / 2));
        gpio_clr_mask(1 << 10);
        gpio_set_function(10, GPIO_FUNC_PIO1);
        
        // Warning: Loser timing hack stalls lower priority ISR! (Hacked/fixed with multicore)
        //  Blocking in ISR is very bad practice. However this logic is simpler this way.
        //  This causes less issues in BCM. Here it can create untracked timing issue for FPS.
        //  Working around this could be a challenge and may create more overhead. It is possible.
        //  Overhead is not as much of a concern for larger notions of blanking time.
        while((time_us_64() - time) < (uint32_t) std::min(2, BLANK_TIME));  // Check if timer has expired
        
        // Kick off hardware to get ISR ticks (GCLK)
        pio_sm_put(pio1, 3, 512);
        pio_interrupt_clear(pio1, 1);
    }
}
