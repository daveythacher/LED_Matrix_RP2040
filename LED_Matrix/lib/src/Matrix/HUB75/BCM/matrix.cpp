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
#include "hardware/structs/bus_ctrl.h"
#include "Matrix/config.h"
#include "Matrix/matrix.h"
#include "Matrix/HUB75/BCM/memory_format.h"
#include "Multiplex/Multiplex.h"
#include "Serial/config.h"
#include "Matrix/HUB75/hw_config.h"
#include "Matrix/HUB75/BCM/Programs.h"

namespace Matrix::Worker {
    extern Matrix::Buffer *get_front_buffer();
};

namespace Matrix::Calculator {
    extern void verify_configuration();
};

namespace Matrix::Worker {
    extern Matrix::Buffer buf[Serial::num_framebuffers];

    extern Matrix::Buffer *get_front_buffer(uint8_t *id);
};

namespace Matrix {
    static Buffer *buffer = nullptr;
    static int dma_chan[4];
    static uint8_t bank;
    static Programs::Ghost_Packet ghost_packet;

    // PIO Protocol
    //  There are 2^PWM_bits shifts per period.
    //      The serial protocol used by PIO is column length decremented by one followed by column values.
    //          The PIO logic is indexed from zero, and there is no way to command zero transfer length.
    //      The serial protocol has a header which indicates the number of transfers to expect
    //          This is loaded by the CPU manually before starting DMA (excludes null termination transfer)
    //  There are 2^PWM_bits plus two transfers.
    //      The second to last transfer turns the columns off before multiplexing. (Standard shift)
    //      The last transfer stops the DMA and fires an interrupt.
    static volatile struct {volatile uint32_t len; volatile uint8_t *data;} address_table[Serial::num_framebuffers][(MULTIPLEX * ((1 << PWM_bits) + 2)) + 1];
    static volatile uint8_t null_table[COLUMNS + 1];
    static uint8_t header = 1 << PWM_bits;              // This needs to be one less than (n + 1)

    static void send_buffer();

    void start() {
        // Init Matrix hardware
        // IO
        for (int i = 0; i < Matrix::HUB75::HUB75_DATA_LEN; i++) {
            gpio_init(i + Matrix::HUB75::HUB75_DATA_BASE);
            gpio_set_dir(i + Matrix::HUB75::HUB75_DATA_BASE, GPIO_OUT);
            gpio_set_function(i + Matrix::HUB75::HUB75_DATA_BASE, GPIO_FUNC_PIO0);
        }
        gpio_init(Matrix::HUB75::HUB75_OE);
        gpio_set_dir(Matrix::HUB75::HUB75_OE, GPIO_OUT);
        gpio_clr_mask(0x40FF00);

        Multiplex::init(Programs::WAKE_MULTIPLEX, Programs::WAKE_GHOST);

        // Promote the CPUs (Branches break sequential/stripping pattern)
        //  CPUs now have 50 percent chance of winning.
        //      They now have 1 turn loss max penalty. 
        //      Performance is 0.5 to 1
        //  DMA now has 50 percent chance of losing.
        //      They now have 3+ turn loss max penalty. 
        //      Performance is <0.25 to 1
        bus_ctrl_hw->priority = (1 << 4) | (1 << 0);
       
        // Do not connect the dots (LEDs), charge the low side before scanning (This will turn the LEDs off)
        //  Do use Dot correction though, which is above this implementation layer
        memset((void *) null_table, 0, COLUMNS + 1);
        null_table[0] = COLUMNS - 1;


        { // Keep stack and variable scope clean
            uint32_t y;

            for (uint8_t b = 0; b < Serial::num_framebuffers; b++) {
                for (uint32_t x = 0; x < MULTIPLEX; x++) {
                    y = x * ((1 << PWM_bits) + 2);
                    address_table[b][y].data = &header;
                    address_table[b][y].len = 1;
                    y += 1;

                    for (uint32_t i = 0; i < PWM_bits; i++) {
                        for (uint32_t k = 0; k < (uint32_t) (1 << i); k++) {
                            address_table[b][y + (1 << i) + k - 1].data = buffer->get_line(x, k);
                        }

                        address_table[b][y + i].len = Buffer::get_line_length();
                    }
                    
                    y += 1 << PWM_bits;
                    address_table[b][y].data = null_table;
                    address_table[b][y].len = COLUMNS + 1;
                }
            
                y += 1;
                address_table[b][y].data = NULL;
                address_table[b][y].len = 0;
            }
        }

        {   // We use a decent amount of stack here (The compiler should figure it out)
            uint16_t instructions[32];
            uint8_t length = Programs::get_pmp_program(instructions, 32);

            static const struct pio_program pio_programs = {
                .instructions = instructions,
                .length = length,
                .origin = 0,
            };
            pio_add_program(pio0, &pio_programs);
            pio_sm_set_consecutive_pindirs(pio0, 0, Matrix::HUB75::HUB75_DATA_BASE, Matrix::HUB75::HUB75_DATA_LEN, true);
        }

        {   // We use a decent amount of stack here (The compiler should figure it out)
            uint16_t instructions[32];
            uint8_t length = Programs::get_ghost_program(instructions, 32);

            static const struct pio_program pio_programs = {
                .instructions = instructions,
                .length = length,
                .origin = 0,
            };
            pio_add_program(pio0, &pio_programs);
            // TODO: Finish
        }

        // TODO: Ghosting program
        
        // Verify Serial Clock
        constexpr float x = 125000000.0 / (SERIAL_CLOCK * 2.0);     // Someday this two will be a four.
        static_assert(x >= 1.0, "Unabled to configure PIO for SERIAL_CLOCK");

        Calculator::verify_configuration();

        // PMP / SM
        pio0->sm[0].clkdiv = ((uint32_t) floor(x) << PIO_SM0_CLKDIV_INT_LSB) | ((uint32_t) round((x - floor(x)) * 255.0) << PIO_SM0_CLKDIV_FRAC_LSB);
        pio0->sm[0].pinctrl = (1 << PIO_SM0_PINCTRL_SIDESET_COUNT_LSB) | (6 << PIO_SM0_PINCTRL_OUT_COUNT_LSB) | ((Matrix::HUB75::HUB75_DATA_BASE + 6) << PIO_SM0_PINCTRL_SIDESET_BASE_LSB) | (Matrix::HUB75::HUB75_DATA_BASE << PIO_SM0_PINCTRL_OUT_BASE_LSB);
        pio0->sm[0].shiftctrl = (1 << PIO_SM0_SHIFTCTRL_AUTOPULL_LSB) | (6 << PIO_SM0_SHIFTCTRL_PULL_THRESH_LSB) | (1 << PIO_SM0_SHIFTCTRL_OUT_SHIFTDIR_LSB);
        pio0->sm[0].execctrl = (1 << PIO_SM1_EXECCTRL_OUT_STICKY_LSB) | (12 << PIO_SM1_EXECCTRL_WRAP_TOP_LSB);
        pio0->sm[0].instr = pio_encode_jmp(0);
        hw_set_bits(&pio0->ctrl, 1 << PIO_CTRL_SM_ENABLE_LSB);
        pio_sm_claim(pio0, 0);
        
        // DMA
        dma_chan[0] = dma_claim_unused_channel(true);
        dma_chan[1] = dma_claim_unused_channel(true);
        dma_chan[2] = dma_claim_unused_channel(true);
        dma_chan[3] = dma_claim_unused_channel(true);
        dma_channel_config c = dma_channel_get_default_config(dma_chan[0]);
        channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
        channel_config_set_read_increment(&c, true);
        channel_config_set_high_priority(&c, true);
        channel_config_set_dreq(&c, DREQ_PIO0_TX0);
        channel_config_set_chain_to(&c, dma_chan[1]);
        channel_config_set_irq_quiet(&c, true);
        dma_channel_configure(dma_chan[0], &c, &pio0_hw->txf[0], NULL, 0, false);
        dma_channel_set_irq0_enabled(dma_chan[0], true); 
        
        c = dma_channel_get_default_config(dma_chan[1]);
        channel_config_set_transfer_data_size(&c, DMA_SIZE_32);
        channel_config_set_read_increment(&c, true);
        channel_config_set_write_increment(&c, true);
        channel_config_set_high_priority(&c, true);
        channel_config_set_ring(&c, true, 3);                                       // 1 << 3 byte boundary on write ptr
        dma_channel_configure(dma_chan[1], &c, &dma_hw->ch[dma_chan[0]].al3_transfer_count, &address_table[bank][0], 2, false);
        
        c = dma_channel_get_default_config(dma_chan[2]);
        channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
        channel_config_set_read_increment(&c, true);
        channel_config_set_write_increment(&c, false);
        channel_config_set_high_priority(&c, false);   
        channel_config_set_dreq(&c, DREQ_PIO0_TX1); 
        channel_config_set_chain_to(&c, dma_chan[3]);

        c = dma_channel_get_default_config(dma_chan[3]);
        channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
        channel_config_set_read_increment(&c, true);
        channel_config_set_write_increment(&c, false);
        channel_config_set_high_priority(&c, false);   
        channel_config_set_dreq(&c, DREQ_PIO0_TX1); 
        channel_config_set_chain_to(&c, dma_chan[2]);
        dma_channel_configure(dma_chan[3], &c, &pio0_hw->txf[1], &ghost_packet, 2, false);

        do {
            buffer = Worker::get_front_buffer(&bank);
        } while (buffer == nullptr);
        
        send_buffer();
        dma_channel_configure(dma_chan[2], &c, &pio0_hw->txf[1], &ghost_packet, 2, true);
    }

    void __not_in_flash_func(send_buffer)() {
        dma_hw->ints0 = 1 << dma_chan[0];
        dma_channel_set_read_addr(dma_chan[1], address_table[bank], true);
    }

    void __not_in_flash_func(dma_isr)() {
        if (dma_channel_get_irq0_status(dma_chan[0])) {
            uint8_t temp;
            Buffer *p = Worker::get_front_buffer(&temp);

            if (p != nullptr) {
                buffer = p;
                bank = temp;
            }

            send_buffer();                                                          // Kick off hardware
        }
    }
}
