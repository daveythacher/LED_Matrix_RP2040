/* 
 * File:   Direct.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "Multiplex/Multiplex.h"
#include "Multiplex/HUB75/hw_config.h"
#include "Multiplex/Programs.h"
#include "Multiplex/Mapper/Mapper.h"

namespace Multiplex {
    static Multiplex_Packet mapper;

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
    }
}
