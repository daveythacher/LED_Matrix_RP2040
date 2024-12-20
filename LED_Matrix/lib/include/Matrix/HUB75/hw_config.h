/* 
 * File:   hw_config.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef MATRIX_HW_CONFIG_H
#define MATRIX_HW_CONFIG_H

#include <stdint.h>

// SIMD_QUARTER 32-bit (Supported)
// SIMD_HALF    64-bit
// SIMD_SINGLE  128-bit (Supported)
// SIMD_DOUBLE  256-bit
// SIMD_QUAD    512-bit

#define HUB75_UNIT  uint8_t         // Cannot be larger than SIMD_UNIT
#define SIMD_UNIT   SIMD_QUARTER    // Make as large as hardware supports

namespace Matrix::HUB75 {
    constexpr uint16_t HUB75_DATA_BASE = 8;
    constexpr uint16_t HUB75_OE = 22;

    // -- DO NOT EDIT BELOW THIS LINE --

    // Order (LSB to MSB): R0 G0 B0 R1 G1 B1 CLK LAT
    constexpr uint16_t HUB75_DATA_LEN = (((sizeof(HUB75_UNIT) * 8) / 6) * 6) + 2;
}

#endif
