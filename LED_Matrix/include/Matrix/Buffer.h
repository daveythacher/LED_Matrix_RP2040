/* 
 * File:   Buffer.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef MATRIX_BUFFER_H
#define MATRIX_BUFFER_H

#include <stdint.h>
#include "Serial/config.h"

namespace Matrix {
    // Currently we only support 8-bit port
    struct Buffer {
        public:
            Buffer();

            void set_value(uint8_t multiplex, uint16_t index, uint8_t column, uint8_t value);
            uint8_t *get_line(uint8_t multiplex, uint16_t index);
            
            static uint8_t get_line_length();

        private:
            uint8_t buf[Serial::max_framebuffer_size];
    };
}

#endif
