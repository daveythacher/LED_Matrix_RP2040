/* 
 * File:   config.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef MATRIX_CONFIG_H
#define MATRIX_CONFIG_H

#include <stdint.h>

// CMake could do this for build automation
//  However verification is manual currently.

#define HARDWARE    BUS8
#define ALGORITHM   PWM         // Matrix
#define MULTIPLEXER Decoder     // Multiplex

namespace Matrix {
    // These are not to exceed 4096 when multiplied together.
    constexpr uint8_t MULTIPLEX = 8;
    constexpr uint16_t STEPS = 64;
    constexpr uint8_t COLUMNS = 8;

    constexpr uint8_t BLANK_TIME = 10;
}

#endif
