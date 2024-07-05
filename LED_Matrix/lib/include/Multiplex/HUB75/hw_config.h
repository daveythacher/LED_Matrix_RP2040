/* 
 * File:   hw_config.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef MULTIPLEX_HW_CONFIG_H
#define MULTIPLEX_HW_CONFIG_H

#include <stdint.h>

namespace Multiplex::HUB75 {
    constexpr uint16_t HUB75_ADDR_BASE = 16;

    // -- DO NOT EDIT BELOW THIS LINE --

    // Order (LSB to MSB): A B C D E
    constexpr uint16_t HUB75_ADDR_LEN = 5;
}

#endif
