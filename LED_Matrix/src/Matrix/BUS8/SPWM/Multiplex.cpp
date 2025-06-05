/* 
 * File:   Multiplex.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include <stdint.h>
#include <string.h>
#include <math.h>
#include "pico/platform.h"
#include "hardware/pio.h"
#include "hardware/gpio.h"
#include "hardware/dma.h"
#include "pico/multicore.h"
#include "hardware/structs/bus_ctrl.h"
#include "Matrix/BUS8/SPWM/Multiplex.h"
#include "Multiplex/Multiplex.h"
#include "Matrix/BUS8/SPWM/Programs.h"
#include "Matrix/BUS8/hw_config.h"
#include "System/machine.h"

namespace Matrix::BUS8::SPWM {
    // PIO Protocol
    //  There are 2^PWM_bits shifts per period.
    //      The serial protocol used by PIO is column length decremented by one followed by column values.
    //          The PIO logic is indexed from zero, and there is no way to command zero transfer length.
    //      The serial protocol has a header which indicates the number of transfers to expect
    //          This is loaded by the CPU manually before starting DMA (excludes null termination transfer)
    //  There are 2^PWM_bits plus two transfers.
    //      The second to last transfer turns the columns off before multiplexing. (Standard shift)
    //      The last transfer stops the DMA and fires an interrupt
    Multiplex::Multiplex() {
        // Init Matrix hardware
        // IO
        for (int i = 0; i < ::Matrix::BUS8::BUS8_DATA_LEN; i++) {
            gpio_init(i + ::Matrix::BUS8::BUS8_DATA_BASE);
            gpio_set_dir(i + ::Matrix::BUS8::BUS8_DATA_BASE, GPIO_OUT);
            gpio_set_function(i + ::Matrix::BUS8::BUS8_DATA_BASE, GPIO_FUNC_PIO0);
        }

        gpio_init(::Matrix::BUS8::BUS8_OE);
        gpio_init(::Matrix::BUS8::BUS8_FCLK);
        gpio_set_dir(::Matrix::BUS8::BUS8_OE, GPIO_OUT);
        gpio_set_dir(::Matrix::BUS8::BUS8_FCLK, GPIO_IN);
        gpio_set_function(::Matrix::BUS8::BUS8_OE, GPIO_FUNC_SIO);
        gpio_set_function(::Matrix::BUS8::BUS8_FCLK, GPIO_FUNC_SIO);

        // Watchdog can see this. (Synchronous)
        ::Multiplex::Multiplex::create_multiplex(Programs::WAKE_MULTIPLEX, Programs::WAKE_GHOST);
        
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
        memset(null_table, 0, sizeof(null_table));
        null_table[0] = COLUMNS - 1;

        header = STEPS_MAJOR + 1;                                                                   // This needs to be one less than (n + 1)
        counter = 0;
        bank = 0;

        {   // We use a decent amount of stack here (The compiler should figure it out)
            uint16_t instructions[32];
            uint8_t length = Programs::get_pmp_program(instructions, 32);

            static const struct pio_program pio_programs = {
                .instructions = instructions,
                .length = length,
                .origin = 0,
            };
            uint offset = pio_add_program(pio0, &pio_programs);
            pio_sm_set_consecutive_pindirs(pio0, 0, ::Matrix::BUS8::BUS8_DATA_BASE, ::Matrix::BUS8::BUS8_DATA_LEN, true);
        
            // Verify Serial Clock
            constexpr float x = 125000000.0 / (SERIAL_CLOCK * 2.0);                                 // Someday this two will be a four.

            // PMP / SM
            pio0->sm[0].clkdiv = ((uint32_t) floor(x) << PIO_SM0_CLKDIV_INT_LSB) | ((uint32_t) round((x - floor(x)) * 255.0) << PIO_SM0_CLKDIV_FRAC_LSB);
            pio0->sm[0].pinctrl = (1 << PIO_SM0_PINCTRL_SIDESET_COUNT_LSB) | (6 << PIO_SM0_PINCTRL_OUT_COUNT_LSB) | ((::Matrix::BUS8::BUS8_DATA_BASE + 6) << PIO_SM0_PINCTRL_SIDESET_BASE_LSB) | (::Matrix::BUS8::BUS8_DATA_BASE << PIO_SM0_PINCTRL_OUT_BASE_LSB);
            pio0->sm[0].shiftctrl = (1 << PIO_SM0_SHIFTCTRL_AUTOPULL_LSB) | (6 << PIO_SM0_SHIFTCTRL_PULL_THRESH_LSB) | (1 << PIO_SM0_SHIFTCTRL_OUT_SHIFTDIR_LSB);
            pio0->sm[0].execctrl = (1 << PIO_SM1_EXECCTRL_OUT_STICKY_LSB) | (12 << PIO_SM1_EXECCTRL_WRAP_TOP_LSB);
            pio0->sm[0].instr = pio_encode_jmp(offset);
            hw_set_bits(&pio0->ctrl, 1 << PIO_CTRL_SM_ENABLE_LSB);
            pio_sm_claim(pio0, 0);
        }

        {   // We use a decent amount of stack here (The compiler should figure it out)
            uint16_t instructions[32];
            uint8_t length = Programs::get_ghost_program(instructions, 32);

            static const struct pio_program pio_programs = {
                .instructions = instructions,
                .length = length,
                .origin = 0,
            };
            uint offset = pio_add_program(pio0, &pio_programs);

            // Ghosting program
            pio0->sm[1].clkdiv = (1 << PIO_SM0_CLKDIV_INT_LSB);
            pio0->sm[1].pinctrl = (1 << PIO_SM0_PINCTRL_SIDESET_COUNT_LSB) | (::Matrix::BUS8::BUS8_OE << PIO_SM0_PINCTRL_SIDESET_BASE_LSB);
            pio0->sm[1].shiftctrl = (1 << PIO_SM0_SHIFTCTRL_OUT_SHIFTDIR_LSB);
            pio0->sm[1].execctrl = (31 << PIO_SM1_EXECCTRL_WRAP_TOP_LSB);
            pio0->sm[1].instr = pio_encode_jmp(offset);
            hw_set_bits(&pio0->ctrl, 2 << PIO_CTRL_SM_ENABLE_LSB);
            pio_sm_claim(pio0, 1);
        }
        
        // DMA
        dma_chan[0] = dma_claim_unused_channel(true);
        dma_chan[1] = dma_claim_unused_channel(true);
        dma_chan[2] = dma_claim_unused_channel(true);
        dma_chan[3] = dma_claim_unused_channel(true);
        dma_channel_config c = dma_channel_get_default_config(dma_chan[0]);
        channel_config_set_transfer_data_size(&c, DMA_SIZE_16);
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
        channel_config_set_ring(&c, true, 3);                                           // 1 << 3 byte boundary on write ptr
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
    }

    void Multiplex::show(Payload *packet) {
        full_pipeline_sync();
        
        if (packet != nullptr) {
            while (!multicore_fifo_wready()) {                                                      // Watchdog can see this. (Synchronous)
                // Do nothing
            }

            multicore_fifo_push_blocking(reinterpret_cast<uint32_t>(packet));                       // Translate
        }

        while (multicore_fifo_rvalid()) {                                                       // Watchdog can see this. (Synchronous)
            delete reinterpret_cast<Payload *>(multicore_fifo_pop_blocking());                  // Translate
         }      
    }

    void __not_in_flash_func(Multiplex::send_buffer)() {
        full_pipeline_sync();
        
        dma_hw->ints0 = 1 << dma_chan[0];
        dma_channel_set_read_addr(dma_chan[1], address_table[bank], true);
        dma_channel_set_read_addr(dma_chan[3], &ghost_packet, true);
    }

    void Multiplex::load_buffer(Payload *packet) {
        uint32_t y;

        for (uint32_t c = 0; c < STEPS_MINOR; c++) {
            for (uint32_t x = 0; x < MULTIPLEX; x++) {
                // [(STEPS_MINOR * MULTIPLEX * (STEPS_MAJOR + 3)) + 1]
                y = (c * MULTIPLEX * (STEPS_MAJOR + 3)) + (x * (STEPS_MAJOR + 3));
                address_table[counter][y].data = &header;               // Control variable: PIO loop counter
                address_table[counter][y].len = 1;
                y += 1;

                for (uint32_t i = 0; i < (STEPS_MAJOR + 1); i++) {      // Data: LED values
                    address_table[counter][y + i].data = packet->get_line(x, i);
                    address_table[counter][y + i].len = packet->get_line_length();
                }
                        
                y += STEPS_MAJOR + 1;
                address_table[counter][y].data = null_table;            // Data: Turns LEDs off
                address_table[counter][y].len = COLUMNS + 1;
            }
        }

        address_table[counter][y + 1].data = NULL;                      // Control variable: Terminate DMA
        address_table[counter][y + 1].len = 0;
        packets[counter] = packet;
        counter = (counter + 1) % num_buffers;
    }

    // Warning: we are high priority here.
    //  We need to be called two times the refresh rate at least.
    //  If we get called to often we stall.
    void __not_in_flash_func(Multiplex::work)() {
        bool swapable = false;

        while (1) {                                                                                 // Wait for first packet
            if (multicore_fifo_rvalid()) {
                load_buffer(reinterpret_cast<Payload *>(multicore_fifo_pop_blocking()));             // Translate
                send_buffer();
                break;
            }
        }

        while (1) {
            if (dma_channel_get_irq0_status(dma_chan[0])) {
                if (swapable && gpio_get(::Matrix::BUS8::BUS8_FCLK)) {
                    while (!multicore_fifo_wready()) {                                              // Watchdog can see this. (Synchronous)
                        // Do nothing
                    }

                    multicore_fifo_push_blocking(reinterpret_cast<uint32_t>(packets[bank]));        // Translate
                    bank = (bank + 1) % num_buffers;
                }

                send_buffer();                                                                      // Kick off hardware
            }

            if (multicore_fifo_rvalid()) {                                                          // Try to get ahead
                if (bank != counter) {                                                              // Watchdog can see this. (Synchronous)
                    load_buffer(reinterpret_cast<Payload *>(multicore_fifo_pop_blocking()));         // Translate
                    swapable = true;
                }
            }
        }
    }
}
