/* 
 * File:   Buffer.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include <string.h>
#include "pico/multicore.h"
#include "Matrix/HUB75/PWM/PWM_Buffer"
#include "Matrix/HUB75/PWM/memory_format.h"

// Every line starts with a counter variable indexed from zero instead of one

namespace Matrix {
    Buffer::Buffer() {
        // Fill in counter variable
        memset(buf, COLUMNS - 1, sizeof(buf));
    }

    void Buffer::set_value(uint8_t multiplex, uint16_t index, uint8_t column, uint8_t value) {
        uint32_t i = multiplex * (1 << PWM_bits) * (COLUMNS + 1);
        i += index * (COLUMNS + 1);
        i += column;

        buf[i] = value;
    }

    uint8_t *Buffer::get_line(uint8_t multiplex, uint16_t index) {
        uint32_t i = multiplex * (1 << PWM_bits) * (COLUMNS + 1);
        i += index * (COLUMNS + 1);

        return &buf[i];
    }

    uint8_t Buffer::get_line_length() {
        return COLUMNS + 1;
    }
}
