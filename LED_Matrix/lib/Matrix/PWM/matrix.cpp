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
#include "Matrix/PWM/memory_format.h"
#include "Multiplex/Multiplex.h"

test2 buf[3];
static uint8_t bank = 0;
static int dma_chan[2];
static struct {uint32_t len; uint8_t *data;} address_table[(1 << PWM_bits) + 2];
static uint8_t null_table[COLUMNS + 1];

static void send_line();
static void load_line(uint32_t rows, uint8_t buffer);

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
    
    memset(buf, COLUMNS - 1, sizeof(buf));
    memset(null_table, 0, COLUMNS + 1);
    null_table[0] = COLUMNS - 1;

    for (uint32_t i = 0; i < (1 << PWM_bits); i++)
        address_table[i].len = COLUMNS + 1;

    address_table[1 << PWM_bits].data = null_table;
    address_table[1 << PWM_bits].len = COLUMNS + 1;
    address_table[(1 << PWM_bits) + 1].data = NULL;
    address_table[(1 << PWM_bits) + 1].len = 0;
    
    // Hack to lower the ISR tick rate, accelerates by 2^PWM_bits (Improves refresh performance)
    //  Automates CLK and LAT signals with DMA and PIO to handle Software PWM of entire row
    //      Works like Hardware PWM without the high refresh
    //      This is more or less how it would work with MACHXO2 FPGA and PIC32MX using PMP.
    //          Bus performance is better with RP2040. (Lower cost due to memory, CPU, hardware integration.)
    //  OE is not used in this implementation and held to low to enable the display
    //      Last shift will disable display.
    /*while (1) {
        counter2 = (1 << PWM_bits) - 1; LAT = 0;    // Start of frame, manually push into FIFO (data stream protocol)
        do {
            counter = COLUMNS - 1;                  // Start of payload, DMA push into FIFO (data stream protocol)
            do {
                DAT = DATA; CLK = 0;                // Payload data, DMA push into FIFO (data stream protocol)
                CLK = 1;                            // Automate CLK pulse
            } while (counter-- > 0); CLK = 0;
            LAT = 1;                                // Automate LAT pulse at end of payload (bitplane shift)
            LAT = 0;
        } while (counter2-- > 0);
        IRQ = 1;                                    // Call CPU at end of frame
    }*/
    
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
    static_assert(x2 >= 1.0, "SERIAL_CLOCK too low");
    constexpr float x = x2 * 125000000.0 / (SERIAL_CLOCK * 2.0);
    static_assert(x >= 1.0, "Unabled to configure PIO for SERIAL_CLOCK");

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
    dma_chan[0] = dma_claim_unused_channel(true);
    dma_chan[1] = dma_claim_unused_channel(true);
    dma_channel_config c = dma_channel_get_default_config(dma_chan[0]);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
    channel_config_set_read_increment(&c, true);
    channel_config_set_dreq(&c, DREQ_PIO0_TX0);
    channel_config_set_chain_to(&c, dma_chan[1]);
    channel_config_set_irq_quiet(&c, true);
    dma_channel_configure(dma_chan[0], &c, &pio0_hw->txf[0], NULL, 0, false);
    dma_channel_set_irq0_enabled(dma_chan[0], true); 
    
    c = dma_channel_get_default_config(dma_chan[1]);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_32);
    channel_config_set_read_increment(&c, true);
    channel_config_set_write_increment(&c, true);
    channel_config_set_ring(&c, true, 3);                                       // 1 << 3 byte boundary on write ptr
    dma_channel_configure(dma_chan[1], &c, &dma_hw->ch[dma_chan[0]].al3_transfer_count, &address_table[0], 2, false);
    
    load_line(0, 1);
    send_line();
}

void __not_in_flash_func(send_line)() {
    dma_hw->ints0 = 1 << dma_chan[0];
    pio_sm_put(pio0, 0, 1 << PWM_bits);
    dma_channel_set_read_addr(dma_chan[1], &address_table[0], true);
}

// This is done to reduce interrupt rate. Use DMA to automate the PWM bitplanes instead of CPU.
//  This is possible due to PIO state machine.
void __not_in_flash_func(load_line)(uint32_t rows, uint8_t buffer) {
    for (uint32_t i = 0; i < (1 << PWM_bits); i++)
            address_table[i].data = buf[buffer][rows][i];
}

void __not_in_flash_func(matrix_dma_isr)() {
    static uint32_t rows = 0;
    extern volatile bool vsync;
    
    if (dma_channel_get_irq0_status(dma_chan[0])) {
        uint64_t time;
        
        // Make sure the FIFO is empty (Warning this will block for a long time if SERIAL_CLOCK is low!)
        //  Warning bug exists with LAT automation when SERIAL_CLOCK is low. (Use assembly delay based on SERIAL_CLOCK? Not needed for this, most of the time.)
        //  ISR already has undefined behavior from ISR blocking (BLANK_TIME), which is very bad! (Expected to small and likely okay for this.)
        while(!pio_sm_is_tx_fifo_empty(pio0, 0));                               
        time = time_us_64();                                                    // Start a timer with 1uS delay
        
        if (++rows >= MULTIPLEX) {                                              // Fire rate: MULTIPLEX * REFRESH
            rows = 0;
            if (vsync) {
                bank = (bank + 1) % 3;
                vsync = false;
            }
        }
        SetRow(rows);
        load_line(rows, bank);                                                  // Note this is a fairly expensive operation. This is done in parallel with blank time.
                                                                                //  This is why no ISR for long delays has been implemented. (It is not believed to be required.)

        while((time_us_64() - time) < BLANK_TIME);                              // Check if timer has expired (TODO: Implement ISR for long delays, if needed.)
        send_line();                                                            // Kick off hardware
    }
}

