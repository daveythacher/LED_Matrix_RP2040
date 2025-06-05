/* 
 * File:   hw_config.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef MATRIX_BUS8_HW_CONFIG_H
#define MATRIX_BUS8_HW_CONFIG_H

#include <stdint.h>
#include "Matrix/config.h"

namespace Matrix::BUS8 { 
    constexpr uint16_t BUS8_DATA_BASE = 8;
    constexpr uint16_t BUS8_OE = 22;
    constexpr uint16_t BUS8_FCLK = 6;

    // Order (LSB to MSB): D0 D1 D2 D3 D4 D5 D6 D7 CLK LAT
    constexpr uint16_t BUS8_DATA_LEN = 10;

    constexpr uint32_t SERIAL_CLOCK = 8333333;
}

#endif
