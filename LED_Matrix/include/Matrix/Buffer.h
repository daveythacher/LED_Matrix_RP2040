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
            ~Buffer();

            static Buffer<T> *create_buffer(uint8_t scan, uint8_t columns);

            void set(uint8_t x, uint8_t y, T *v);
            void ntoh();
        
        private:
            Buffer();
            Buffer(uint8_t scan, uint8_t columns);

            uint8_t _scan;
            uint8_t _columns;
            T *_buffer;
    };
}

#endif

