/* 
 * File:   PWM_Buffer.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include <string.h>
#include "pico/multicore.h"
#include "Matrix/HUB75/PWM/PWM_Buffer.h"
#include "Matrix/HUB75/PWM/memory_format.h"

// Every line starts with a counter variable indexed from zero instead of one

namespace Matrix {
    template <typename T> PWM_Buffer<T>::PWM_Buffer() {}

    template <typename T> PWM_Buffer<T>::PWM_Buffer(uint8_t scan, uint16_t steps, uint8_t columns) {
        _scan = scan;
        _columns = columns;
        _steps = steps;
        _buffer = new T[scan * steps * columns];
    }

    template <typename T> PWM_Buffer<T>::~PWM_Buffer() {
        delete[] _buffer;
    }

    template <typename T> PWM_Buffer<T> *create_pwm_buffer(uint8_t scan, uint16_t steps, uint8_t columns) {
        return new PWM_Buffer<T>(scan, steps, columns);
    }

    template <typename T> void PWM_Buffer<T>::set_value(uint8_t multiplex, uint16_t index, uint8_t column, T value) {
        if (multiplex > _scan || index > _steps || column > _columns)
            return;

        uint32_t i = multiplex * _steps * (_columns + 1);
        i += index * (_columns + 1);
        i += column;

        _buffer[i] = value;
    }

    template <typename T> T *PWM_Buffer<T>::get_line(uint8_t multiplex, uint16_t index) {
        if (multiplex > _scan || index > _steps)
            return;

        uint32_t i = multiplex * _steps * (_columns + 1);
        i += index * (_columns + 1);

        return &_buffer[i];
    }

    template <typename T> uint16_t PWM_Buffer<T>::get_line_length() {
        return _columns + 1;
    }
}
