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
    // Only used by BUS8::SPWM
    constexpr uint16_t STEPS_MAJOR = 16;
    constexpr uint16_t STEPS_MINOR = 64;

    // These are not to exceed 4096 when multiplied together.
    constexpr uint8_t MULTIPLEX = 8;
    constexpr uint16_t STEPS = 64;                                  // For BUS8::SPWM: STEPS_MAJOR + STEPS_MINOR
    constexpr uint8_t COLUMNS = 8;

    constexpr uint8_t BLANK_TIME = 10;
    constexpr uint32_t SERIAL_CLOCK = 15000000;
}

#endif
