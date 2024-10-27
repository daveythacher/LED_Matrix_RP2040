/* 
 * File:   Buffer.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include <string.h>
#include "pico/multicore.h"
#include "Matrix/Buffer.h"
#include "Matrix/HUB75/BCM/memory_format.h"

// Every line starts with a counter variable indexed from zero instead of one

namespace Matrix {
    Buffer::Buffer() {
        // Fill in counter variable
        memset(buf, COLUMNS - 1, sizeof(buf));
    }

    void __not_in_flash_func(Buffer::set_value)(uint8_t multiplex, uint16_t index, uint8_t column, uint8_t value) {
        uint32_t i = multiplex * PWM_bits * (COLUMNS + 1);
        i += index * (COLUMNS + 1);
        i += column;

        buf[i] = value;
    }

    uint8_t *__not_in_flash_func(Buffer::get_line)(uint8_t multiplex, uint16_t index) {
        uint32_t i = multiplex * PWM_bits * (COLUMNS + 1);
        i += index * (COLUMNS + 1);

        return &buf[i];
    }

    uint8_t __not_in_flash_func(Buffer::get_line_length)() {
        return COLUMNS + 1;
    }
}
