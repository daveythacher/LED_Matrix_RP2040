/* 
 * File:   hw_config.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef MATRIX_BUS8_HW_CONFIG_H
#define MATRIX_BUS8_HW_CONFIG_H

#include <stdint.h>

namespace Matrix::BUS8 {
    constexpr uint16_t BUS8_DATA_BASE = 8;
    constexpr uint16_t BUS8_OE = 22;

    // Order (LSB to MSB): B0 B1 B2 B3 B4 B5 B6 B7 CLK LAT
    constexpr uint16_t BUS8_DATA_LEN = 10;
}

#endif
