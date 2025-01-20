/* 
 * File:   BCM_Buffer.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef MATRIX_BCM_BUFFER_H
#define MATRIX_BCM_BUFFER_H

#include "Serial/config.h"

namespace Matrix {
    class BCM_Buffer {
        public:
            BCM_Buffer();

            void set_value(uint8_t multiplex, uint16_t index, uint8_t column, uint8_t value);
            uint8_t *get_line(uint8_t multiplex, uint16_t index);
            static uint8_t get_line_length();

            uint8_t *serialize();
            void deserialize(uint8_t *buf, uint16_t len);

        private:
            uint8_t buf[Serial::max_framebuffer_size];
    };
}

#endif
