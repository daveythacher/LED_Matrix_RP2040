/* 
 * File:   hw_config.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef MATRIX_HW_CONFIG_H
#define MATRIX_HW_CONFIG_H

#include <stdint.h>

//          Operation                           Parallel bitplanes
// SIMD_UNIT    Width   Status      uint8_t     uint16_t    uint32_t    uint64_t
// SIMD_QUARTER 32-bit  (Supported)     4           2           1           0
// SIMD_HALF    64-bit                  8           4           2           1
// SIMD_SINGLE  128-bit                 16          8           4           2
// SIMD_DOUBLE  256-bit                 32          16          8           4
// SIMD_QUAD    512-bit                 64          32          16          8

// HUB75_UNIT   Parallel Chains     Pins        RP2040
// uint8_t          1               6           (Supported)
// uin16_t          2               12          (Maybe)
// uint32_t         5               30
// uint64_t         10              60

// Matrix is:
//  PWM: Pins by (2^bits / Parallel bitplanes)
//  BCM: Pins by (bits / Parallel bitplanes)

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
