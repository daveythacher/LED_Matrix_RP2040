/* 
 * File:   Decoder.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
#include "Multiplex/Multiplex.h"
#include "Multiplex/HUB75/hw_config.h"
#include "Multiplex/Programs.h"
#include "Multiplex/Mapper/Mapper.h"

namespace Multiplex {
    static Multiplex_Packet mapper;
    static int dma_chan[2];

    void init(int start_flag, int signal_flag) {
        for (int i = 0; i < Multiplex::HUB75::HUB75_ADDR_LEN; i++) {
            gpio_init(i + Multiplex::HUB75::HUB75_ADDR_BASE);
            gpio_set_dir(i + Multiplex::HUB75::HUB75_ADDR_BASE, GPIO_OUT);
        }
        gpio_clr_mask(0x1F0000);

        mapper = map();

        for (uint8_t i = 0; i < mapper.length; i++) {
            mapper.buffer[i] = mapper.buffer[i] % Matrix::MULTIPLEX;
        }

        {   // We use a decent amount of stack here (The compiler should figure it out)
            uint16_t instructions[32];
            uint8_t length = Programs::get_address_program(instructions, 32);

            static const struct pio_program pio_programs = {
                .instructions = instructions,
                .length = length,
                .origin = 0,
            };
            pio_add_program(pio0, &pio_programs);
            // TODO: Finish
        }

        // TODO:

        // PMP / SM
        pio0->sm[0].clkdiv = (1 << PIO_SM0_CLKDIV_INT_LSB) | (0 << PIO_SM0_CLKDIV_FRAC_LSB);
        pio0->sm[0].pinctrl = (2 << PIO_SM0_PINCTRL_SIDESET_COUNT_LSB) | (6 << PIO_SM0_PINCTRL_OUT_COUNT_LSB) | ((Matrix::HUB75::HUB75_DATA_BASE + 6) << PIO_SM0_PINCTRL_SIDESET_BASE_LSB);
        pio0->sm[0].shiftctrl = (1 << PIO_SM0_SHIFTCTRL_AUTOPULL_LSB) | (6 << PIO_SM0_SHIFTCTRL_PULL_THRESH_LSB) | (1 << PIO_SM0_SHIFTCTRL_OUT_SHIFTDIR_LSB);
        pio0->sm[0].execctrl = (1 << PIO_SM1_EXECCTRL_OUT_STICKY_LSB) | (12 << PIO_SM1_EXECCTRL_WRAP_TOP_LSB);
        pio0->sm[0].instr = pio_encode_jmp(0);
        hw_set_bits(&pio0->ctrl, 1 << PIO_CTRL_SM_ENABLE_LSB);
        pio_sm_claim(pio0, 0);

        // DMA
        dma_chan[0] = dma_claim_unused_channel(true);
        dma_chan[1] = dma_claim_unused_channel(true);
        dma_channel_config c = dma_channel_get_default_config(dma_chan[2]);
        channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
        channel_config_set_read_increment(&c, true);
        channel_config_set_write_increment(&c, false);
        channel_config_set_high_priority(&c, false);   
        channel_config_set_dreq(&c, DREQ_PIO0_TX1); 
        channel_config_set_chain_to(&c, dma_chan[3]);
        dma_channel_configure(dma_chan[2], &c, &pio0_hw->txf[1], mapper.buffer, mapper.length, false);

        c = dma_channel_get_default_config(dma_chan[3]);
        channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
        channel_config_set_read_increment(&c, true);
        channel_config_set_write_increment(&c, false);
        channel_config_set_high_priority(&c, false);   
        channel_config_set_dreq(&c, DREQ_PIO0_TX1); 
        channel_config_set_chain_to(&c, dma_chan[2]);
        dma_channel_configure(dma_chan[3], &c, &pio0_hw->txf[1], mapper.buffer, mapper.length, false);
    }
}
