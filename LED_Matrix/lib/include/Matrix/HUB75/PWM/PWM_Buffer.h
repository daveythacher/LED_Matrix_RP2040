/* 
 * File:   PWM_Buffer.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef MATRIX_PWM_BUFFER_H
#define MATRIX_PWM_BUFFER_H

#include "Matrix/Buffer.h"

namespace Matrix {
    template <typename T> class PWM_Buffer : public Buffer<T> {
        public:
            ~PWM_Buffer();

            static PWM_Buffer *create_pwm_buffer(uint8_t rows, uint8_t columns);

            void set(uint8_t row, uint8_t column, T value);
            T get(uint8_t row, uint8_t column);

        private:
            PWM_Buffer();
            PWM_Buffer(uint8_t scan, uint8_t columns);

            uint8_t _rows;
            uint8_t _columns;
            T *_buffer;
    };
}

#endif
