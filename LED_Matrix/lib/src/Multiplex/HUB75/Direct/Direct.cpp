/* 
 * File:   Direct.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "Multiplex/Multiplex.h"
#include "Multiplex/HUB75/hw_config.h"
#include "hardware/gpio.h"

namespace Multiplex {
    void init(int rows, int start_flag, int signal_flag) {
        for (int i = 0; i < Multiplex::HUB75::HUB75_ADDR_LEN; i++) {
            gpio_init(i + Multiplex::HUB75::HUB75_ADDR_BASE);
            gpio_set_dir(i + Multiplex::HUB75::HUB75_ADDR_BASE, GPIO_OUT);
        }
        gpio_clr_mask(0x1F0000);

        // TODO:
    }
}
