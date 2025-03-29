/* 
 * File:   PWM_Buffer.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "Matrix/HUB75/PWM/PWM_Buffer.h"
#include "Matrix/Types.h"

namespace Matrix {
    template <typename T> PWM_Buffer<T>::PWM_Buffer() {}

    template <typename T> PWM_Buffer<T>::PWM_Buffer(uint8_t rows, uint8_t columns) {
        _rows = rows;
        _columns = columns;
        _buffer = new T[rows * columns];
    }

    template <typename T> PWM_Buffer<T>::~PWM_Buffer() {
        delete[] _buffer;
    }

    template <typename T> PWM_Buffer<T> *create_PWM_Buffer(uint8_t rows, uint8_t columns) {
        return new PWM_Buffer<T>(rows, columns);
    }

    template <typename T> void PWM_Buffer<T>::set(uint8_t row, uint8_t column, T value) {
        if (row > _rows || column > _columns)
            return;

        uint32_t i = (row * _columns) + column;
        _buffer[i] = value;
    }

    template <typename T> T PWM_Buffer<T>::get(uint8_t row, uint8_t column) {
        if (row > _rows || column > _columns)
            return _buffer[0];

        uint32_t i = (row * _columns) + column;
        return _buffer[i];
    }


    template <typename T> uint8_t PWM_Buffer<T>::num_rows() {
        return _rows;
    }

    template <typename T> uint8_t PWM_Buffer<T>::num_columns() {
        return _columns;
    }

    template class PWM_Buffer<RGB24>;
    template class PWM_Buffer<RGB48>;
    template class PWM_Buffer<RGB_222>;
    template class PWM_Buffer<RGB_555>;
}
