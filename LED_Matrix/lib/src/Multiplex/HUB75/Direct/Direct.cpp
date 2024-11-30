/* 
 * File:   Direct.cpp
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
            mapper.buffer[i] = 1 << (mapper.buffer[i] % 5);
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
