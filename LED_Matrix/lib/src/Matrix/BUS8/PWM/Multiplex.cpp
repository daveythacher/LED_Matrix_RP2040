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
#include "hardware/structs/bus_ctrl.h"
#include "Matrix/BUS8/PWM/Multiplex.h"
#include "Multiplex/Multiplex.h"
#include "Matrix/BUS8/PWM/Programs.h"
#include "Matrix/BUS8/hw_config.h"
#include "GPIO/GPIO.h"

namespace Matrix::BUS8::PWM {
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
        header = STEPS;                                                                 // This needs to be one less than (n + 1)
        thread = new Concurrent::Thread(work, 4096, 255, this);
        queue = new Concurrent::Queue<Packet *>(2);
        counter = 0;
        bank = 0;

        // Init Matrix hardware
        // IO
        for (int i = 0; i < ::Matrix::BUS8::BUS8_DATA_LEN; i++) {
            gpio_init(i + ::Matrix::BUS8::BUS8_DATA_BASE);
            gpio_set_dir(i + ::Matrix::BUS8::BUS8_DATA_BASE, GPIO_OUT);
            gpio_set_function(i + ::Matrix::BUS8::BUS8_DATA_BASE, GPIO_FUNC_PIO0);
            IO::GPIO::claim(i + ::Matrix::BUS8::BUS8_DATA_BASE);
        }
        gpio_init(::Matrix::BUS8::BUS8_OE);
        gpio_set_dir(::Matrix::BUS8::BUS8_OE, GPIO_OUT);
        gpio_clr_mask(0x40FFF0);

        gpio_init(::Matrix::BUS8::BUS8_RCLK);
        gpio_init(::Matrix::BUS8::BUS8_FCLK);
        gpio_set_dir(::Matrix::BUS8::BUS8_RCLK, GPIO_IN);
        gpio_set_dir(::Matrix::BUS8::BUS8_FCLK, GPIO_IN);
        gpio_set_function(::Matrix::BUS8::BUS8_RCLK, GPIO_FUNC_SIO);
        gpio_set_function(::Matrix::BUS8::BUS8_FCLK, GPIO_FUNC_SIO);
        IO::GPIO::claim(::Matrix::BUS8::BUS8_RCLK);
        IO::GPIO::claim(::Matrix::BUS8::BUS8_FCLK);

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
        memset((void *) null_table, 0, sizeof(null_table));
        null_table[0] = COLUMNS - 1;

        {   // We use a decent amount of stack here (The compiler should figure it out)
            uint16_t instructions[32];
            uint8_t length = Programs::get_pmp_program(instructions, 32);

            static const struct pio_program pio_programs = {
                .instructions = instructions,
                .length = length,
                .origin = 0,
            };
            pio_add_program(pio0, &pio_programs);
            pio_sm_set_consecutive_pindirs(pio0, 0, ::Matrix::BUS8::BUS8_DATA_BASE, ::Matrix::BUS8::BUS8_DATA_LEN, true);
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
        
        // Verify Serial Clock
        constexpr float x = 125000000.0 / (SERIAL_CLOCK * 2.0);                         // Someday this two will be a four.

        // PMP / SM
        pio0->sm[0].clkdiv = ((uint32_t) floor(x) << PIO_SM0_CLKDIV_INT_LSB) | ((uint32_t) round((x - floor(x)) * 255.0) << PIO_SM0_CLKDIV_FRAC_LSB);
        pio0->sm[0].pinctrl = (1 << PIO_SM0_PINCTRL_SIDESET_COUNT_LSB) | (6 << PIO_SM0_PINCTRL_OUT_COUNT_LSB) | ((::Matrix::BUS8::BUS8_DATA_BASE + 6) << PIO_SM0_PINCTRL_SIDESET_BASE_LSB) | (::Matrix::BUS8::BUS8_DATA_BASE << PIO_SM0_PINCTRL_OUT_BASE_LSB);
        pio0->sm[0].shiftctrl = (1 << PIO_SM0_SHIFTCTRL_AUTOPULL_LSB) | (6 << PIO_SM0_SHIFTCTRL_PULL_THRESH_LSB) | (1 << PIO_SM0_SHIFTCTRL_OUT_SHIFTDIR_LSB);
        pio0->sm[0].execctrl = (1 << PIO_SM1_EXECCTRL_OUT_STICKY_LSB) | (12 << PIO_SM1_EXECCTRL_WRAP_TOP_LSB);
        pio0->sm[0].instr = pio_encode_jmp(0);
        // TODO: Add frame here
        hw_set_bits(&pio0->ctrl, 1 << PIO_CTRL_SM_ENABLE_LSB);
        pio_sm_claim(pio0, 0);

        // TODO: Fix (Ghosting program)
        pio0->sm[1].clkdiv = ((uint32_t) floor(x) << PIO_SM0_CLKDIV_INT_LSB) | ((uint32_t) round((x - floor(x)) * 255.0) << PIO_SM0_CLKDIV_FRAC_LSB);
        pio0->sm[1].pinctrl = (1 << PIO_SM0_PINCTRL_SIDESET_COUNT_LSB) | (6 << PIO_SM0_PINCTRL_OUT_COUNT_LSB) | ((::Matrix::BUS8::BUS8_DATA_BASE + 6) << PIO_SM0_PINCTRL_SIDESET_BASE_LSB) | (::Matrix::BUS8::BUS8_DATA_BASE << PIO_SM0_PINCTRL_OUT_BASE_LSB);
        pio0->sm[1].shiftctrl = (1 << PIO_SM0_SHIFTCTRL_AUTOPULL_LSB) | (6 << PIO_SM0_SHIFTCTRL_PULL_THRESH_LSB) | (1 << PIO_SM0_SHIFTCTRL_OUT_SHIFTDIR_LSB);
        pio0->sm[1].execctrl = (1 << PIO_SM1_EXECCTRL_OUT_STICKY_LSB) | (12 << PIO_SM1_EXECCTRL_WRAP_TOP_LSB);
        pio0->sm[1].instr = pio_encode_jmp(0);
        // TODO: Add Frame here
        hw_set_bits(&pio0->ctrl, 2 << PIO_CTRL_SM_ENABLE_LSB);
        pio_sm_claim(pio0, 1);
        
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

    void Multiplex::show(Packet *packet) {
        // TODO:
    }

    void __not_in_flash_func(Multiplex::send_buffer)() {
        // TODO: Add frame here
        dma_hw->ints0 = 1 << dma_chan[0];
        dma_channel_set_read_addr(dma_chan[1], address_table[bank], true);
        dma_channel_set_read_addr(dma_chan[3], &ghost_packet, true);
    }

    void Multiplex::load_buffer(Packet *packet) {
        uint32_t y;

        for (uint32_t x = 0; x < MULTIPLEX; x++) {
            y = x * (STEPS + 2);
            address_table[counter][y].data = &header;
            address_table[counter][y].len = 1;
            y += 1;

            for (uint32_t i = 0; i < STEPS; i++) {
                address_table[counter][y + i].data = packet->get_line(x, i);
                address_table[counter][y + i].len = packet->get_line_length();
            }
                    
            y += STEPS;
            address_table[counter][y].data = null_table;
            address_table[counter][y].len = COLUMNS + 1;
        }

        y += 1;
        address_table[counter][y + 1].data = NULL;
        address_table[counter][y + 1].len = 0;
        counter = (counter + 1) % 3;
    }

    // Warning: we are high priority here.
    //  We need to be called two times the refresh rate at least.
    //  If we get called to often we stall.
    void __not_in_flash_func(Multiplex::work)(void *arg) {
        bool swapable = false;
        Multiplex *multiplex = static_cast<Multiplex *>(arg);

        while (1) {
            if (multiplex->queue->available()) {
                multiplex->load_buffer(multiplex->queue->pop());
                multiplex->send_buffer();
                break;
            }
        }

        while (1) {
            if (dma_channel_get_irq0_status(multiplex->dma_chan[0])) {
                if (swapable && gpio_get(::Matrix::BUS8::BUS8_FCLK)) {
                    multiplex->bank = (multiplex->bank + 1) % 3;
                }

                while (!gpio_get(::Matrix::BUS8::BUS8_RCLK)) {
                    // Do nothing
                }

                multiplex->send_buffer();                                                          // Kick off hardware
                // TODO: Add watchdog protection
            }

            if (multiplex->queue->available()) {                                                   // Try to get ahead
                if (multiplex->bank == multiplex->counter) {
                    Concurrent::Thread::Yield();
                }
                else {
                    multiplex->load_buffer(multiplex->queue->pop());
                    swapable = true;
                }
            }
        }
    }
}
