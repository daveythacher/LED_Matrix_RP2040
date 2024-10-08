/* 
 * File:   hw_config.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef MATRIX_HW_CONFIG_H
#define MATRIX_HW_CONFIG_H

#include <stdint.h>

namespace Matrix::HUB75 {
    constexpr uint16_t HUB75_DATA_BASE = 8;
    constexpr uint16_t HUB75_OE = 22;

    // -- DO NOT EDIT BELOW THIS LINE --

    // Order (LSB to MSB): R0 G0 B0 R1 G1 B1 CLK LAT
    constexpr uint16_t HUB75_DATA_LEN = 8;
}

#endif
