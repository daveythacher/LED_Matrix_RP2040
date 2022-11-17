/* 
 * File:   Decoder.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "Multiplex/Multiplex.h"
#include "hardware/gpio.h"

void __not_in_flash_func(SetRow)(int row) {
    gpio_clr_mask(0x1F << 16);
    gpio_set_mask((row & 0x1F) << 16);
}

