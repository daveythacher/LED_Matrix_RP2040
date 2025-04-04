/* 
 * File:   Buffer.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef MATRIX_BUFFER_H
#define MATRIX_BUFFER_H

#include <stdint.h>

namespace Matrix {
    // These are used for pixel data. (As sorted in hardware!)
    template <typename T> class Buffer {
        public:
            virtual void set(uint8_t row, uint8_t column, T val) = 0;
            virtual T get(uint8_t row, uint8_t column) = 0;
            virtual uint8_t num_rows() = 0;
            virtual uint8_t num_columns() = 0;
    };
}

#endif

