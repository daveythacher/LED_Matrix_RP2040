/* 
 * File:   Direct.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "Multiplex/Multiplex.h"
#include "Multiplex/HUB75/hw_config.h"
#include "hardware/gpio.h"

namespace Multiplex {
    void init(int start_flag, int signal_flag) {
        for (int i = 0; i < Multiplex::HUB75::HUB75_ADDR_LEN; i++) {
            gpio_init(i + Multiplex::HUB75::HUB75_ADDR_BASE);
            gpio_set_dir(i + Multiplex::HUB75::HUB75_ADDR_BASE, GPIO_OUT);
        }
        gpio_clr_mask(0x1F0000);

        // TODO:
    }

    uint8_t Programs::get_address_program(uint16_t *instructions, uint8_t len) {
        uint8_t size;
        Program PMP(2);     // Warning not all behavior is supported
        ASM program[] = {   // Sidesets are cleared by default
            PULL(true),
            WAIT(Flags::IRQ, true, WAKE_MULTIPLEX),             // Wait till we are called (by Ghost) - single threaded
            OUT(PINS, 5),
            IRQ(true, WAKE_GHOST),                              // Call Ghost - multi-threaded
            JMP(0)
        };

        size = sizeof(program) / sizeof(ASM);
        size = std::min(len, size);
        PMP.replace(program, 0, size);
        PMP.translate(0, instructions, size);
        return size;
    }
}
