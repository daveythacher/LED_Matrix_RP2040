/* 
 * File:   Direct.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "Multiplex/Multiplex.h"
#include "hardware/gpio.h"

namespace Multiplex {
    void init(int rows) {
        for (int i = 0; i < 5; i++) {
            gpio_init(i + 16);
            gpio_set_dir(i + 16, GPIO_OUT);
        }
    }

    void __not_in_flash_func(SetRow)(int row) {
        gpio_clr_mask(0x1F << 16);
        gpio_set_mask(1 + ((row % 5) + 16));
    }
}
