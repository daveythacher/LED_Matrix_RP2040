/* 
 * File:   Buffer.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef MATRIX_BUFFER_H
#define MATRIX_BUFFER_H

#include <stdint.h>

namespace Matrix {
    // These are used for pixel data.
    template <typename T> class Buffer {
        public:
            Buffer();

            void set(uint8_t x, uint8_t y, T *v);
            void ntoh();

        private:
            uint8_t buffer[8 * 1024];
    };
}

#endif

