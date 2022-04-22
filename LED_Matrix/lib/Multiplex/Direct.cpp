/* 
 * File:   Direct.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "Multiplex/Direct.h"
#include "hardware/gpio.h"

void __not_in_flash_func(Direct::SetRow)(int row) {
    gpio_clr_mask(0x1F << 11);
    gpio_set_mask((row % 0x1F) << 11);
}

