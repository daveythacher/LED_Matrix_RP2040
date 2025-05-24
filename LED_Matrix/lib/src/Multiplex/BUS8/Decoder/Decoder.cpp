/* 
 * File:   Decoder.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
#include "Multiplex/Multiplex.h"
#include "Multiplex/BUS8/Decoder/Decoder.h"
#include "Multiplex/BUS8/hw_config.h"
#include "Multiplex/BUS8/Decoder/Programs.h"
#include "GPIO/GPIO.h"

namespace Multiplex::BUS8::Decoder {
    static int dma_chan[2];

    Decoder::Decoder(int start_flag, int signal_flag) {
        for (int i = 0; i < ::Multiplex::BUS8::BUS8_ADDR_LEN; i++) {
            gpio_init(i + ::Multiplex::BUS8::BUS8_ADDR_BASE);
            gpio_set_dir(i + ::Multiplex::BUS8::BUS8_ADDR_BASE, GPIO_OUT);
            gpio_set_function(i + ::Multiplex::BUS8::BUS8_ADDR_BASE, GPIO_FUNC_SIO);
            IO::GPIO::claim(i + ::Multiplex::BUS8::BUS8_ADDR_BASE);
        }
        gpio_clr_mask(0x1F0000);

        for (uint8_t i = 0; i < Matrix::MULTIPLEX; i++) {
            mapper[i] = i;
        }

        {   // We use a decent amount of stack here (The compiler should figure it out)
            uint16_t instructions[32];
            uint8_t length = Programs::get_address_program(instructions, 32, start_flag, signal_flag);

            static const struct pio_program pio_programs = {
                .instructions = instructions,
                .length = length,
                .origin = 0,
            };
            pio_add_program(pio1, &pio_programs);
            pio_sm_set_consecutive_pindirs(pio0, 0, ::Multiplex::BUS8::BUS8_ADDR_BASE, ::Multiplex::BUS8::BUS8_ADDR_LEN, true);
        }

        // TODO:

        // PMP / SM
        pio1->sm[0].clkdiv = (1 << PIO_SM0_CLKDIV_INT_LSB) | (0 << PIO_SM0_CLKDIV_FRAC_LSB);
        pio1->sm[0].pinctrl = (5 << PIO_SM0_PINCTRL_OUT_COUNT_LSB) | ::Multiplex::BUS8::BUS8_ADDR_BASE;
        pio1->sm[0].shiftctrl = 1 << PIO_SM0_SHIFTCTRL_OUT_SHIFTDIR_LSB;
        pio1->sm[0].execctrl = (1 << PIO_SM1_EXECCTRL_OUT_STICKY_LSB) | (0x1F << PIO_SM1_EXECCTRL_WRAP_TOP_LSB);
        pio1->sm[0].instr = pio_encode_jmp(0);
        hw_set_bits(&pio1->ctrl, 1 << PIO_CTRL_SM_ENABLE_LSB);
        pio_sm_claim(pio1, 0);

        // DMA
        dma_chan[0] = dma_claim_unused_channel(true);
        dma_chan[1] = dma_claim_unused_channel(true);
        dma_channel_config c = dma_channel_get_default_config(dma_chan[0]);
        channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
        channel_config_set_read_increment(&c, true);
        channel_config_set_write_increment(&c, false);
        channel_config_set_high_priority(&c, false);   
        channel_config_set_dreq(&c, DREQ_PIO0_TX1); 
        channel_config_set_chain_to(&c, dma_chan[1]);
        dma_channel_configure(dma_chan[0], &c, &pio0_hw->txf[1], mapper, Matrix::MULTIPLEX, false);

        c = dma_channel_get_default_config(dma_chan[1]);
        channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
        channel_config_set_read_increment(&c, true);
        channel_config_set_write_increment(&c, false);
        channel_config_set_high_priority(&c, false);   
        channel_config_set_dreq(&c, DREQ_PIO0_TX1); 
        channel_config_set_chain_to(&c, dma_chan[0]);
        // TODO: Add Frame here
        dma_channel_configure(dma_chan[1], &c, &pio0_hw->txf[1], mapper, Matrix::MULTIPLEX, true);

        // We lock up Multiplex on error
    }

    Decoder *Decoder::create_multiplex(int start_flag, int signal_flag) {
        if (ptr == nullptr) {
            ptr = new Decoder(start_flag, signal_flag);
        }

        return ptr;
    }
}
