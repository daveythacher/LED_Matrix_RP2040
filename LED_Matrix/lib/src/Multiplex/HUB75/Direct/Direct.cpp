/* 
 * File:   Direct.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "Multiplex/Multiplex.h"
#include "Multiplex/HUB75/hw_config.h"
#include "hardware/gpio.h"

namespace Multiplex {
    void init(int rows) {
        for (int i = 0; i < Multiplex::HUB75::HUB75_ADDR_LEN; i++) {
            gpio_init(i + Multiplex::HUB75::HUB75_ADDR_BASE);
            gpio_set_dir(i + Multiplex::HUB75::HUB75_ADDR_BASE, GPIO_OUT);
        }
        gpio_clr_mask(0x1F0000);
    }

    void __not_in_flash_func(SetRow)(int row) {
        int mask = (1 << Multiplex::HUB75::HUB75_ADDR_LEN) - 1;
        gpio_clr_mask(0x1F << Multiplex::HUB75::HUB75_ADDR_BASE);
        gpio_set_mask((row % Multiplex::HUB75::HUB75_ADDR_LEN) + Multiplex::HUB75::HUB75_ADDR_BASE);
    }
}
