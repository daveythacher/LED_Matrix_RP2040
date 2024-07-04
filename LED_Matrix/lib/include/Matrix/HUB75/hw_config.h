/* 
 * File:   hw_config.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef MATRIX_HW_CONFIG_H
#define MATRIX_HW_CONFIG_H

#include <stdint.h>

namespace Matrix::HUB75 {
    // Order (LSB to MSB): R0 G0 B0 R1 G1 B1 CLK LAT
    constexpr uint16_t HUB75_DATA_BASE = 8;
    constexpr uint16_t HUB75_DATA_LEN = 8;

    // Order (LSB to MSB): A B C D E
    constexpr uint16_t HUB75_ADDR_BASE = 16;
    constexpr uint16_t HUB75_ADDR_LEN = 5;

    constexpr uint16_t HUB75_OE = 22;
}

#endif
