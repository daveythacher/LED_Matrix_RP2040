/* 
 * File:   PWM_Packet.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include <string.h>
#include "Matrix/HUB75/PWM/PWM_Packet.h"
#include "Matrix/HUB75/hw_config.h"

// Every line starts with a counter variable indexed from zero instead of one

namespace Matrix {
    template <typename T> PWM_Packet<T>::PWM_Packet() {}

    template <typename T> PWM_Packet<T>::PWM_Packet(uint8_t scan, uint16_t steps, uint8_t columns) {
        _scan = scan;
        _columns = columns;
        _steps = steps;
        _buffer = new T[scan * steps * (columns + 1)];

        memset(_buffer, columns - 1, scan * steps * columns);
    }

    template <typename T> PWM_Packet<T>::~PWM_Packet() {
        delete[] _buffer;
    }

    template <typename T> PWM_Packet<T> *create_pwm_packet(uint8_t scan, uint16_t steps, uint8_t columns) {
        return new PWM_Packet<T>(scan, steps, columns);
    }

    template <typename T> void PWM_Packet<T>::set(uint8_t multiplex, uint16_t index, uint8_t column, T value) {
        if (multiplex > _scan || index > _steps || column > _columns)
            return;

        uint32_t i = multiplex * _steps * (_columns + 1);
        i += index * (_columns + 1);
        i += column;

        _buffer[i + 1] = value;
    }

    template <typename T> T PWM_Packet<T>::get(uint8_t multiplex, uint16_t index, uint8_t column) {
        if (multiplex > _scan || index > _steps || column > _columns)
            return _buffer[1];

        uint32_t i = multiplex * _steps * (_columns + 1);
        i += index * (_columns + 1);
        i += column;

        return _buffer[i + 1];
    }


    template <typename T> uint8_t PWM_Packet<T>::num_scan() {
        return _scan;
    }

    template <typename T> uint16_t PWM_Packet<T>::num_columns(bool isRaw) {
        if (isRaw) {
            return _columns + 1;
        }
        
        return _columns;
    }

    template <typename T> uint32_t PWM_Packet<T>::num_steps() {
        return _steps;
    }

    template <typename T> T *PWM_Packet<T>::get_line(uint8_t multiplex, uint16_t index) {
        if (multiplex > _scan || index > _steps)
            return nullptr;

        uint32_t i = multiplex * _steps * (_columns + 1);
        i += index * (_columns + 1);

        return &_buffer[i];
    }

    template <typename T> uint16_t PWM_Packet<T>::get_line_length() {
        return _columns + 1;
    }

    template class PWM_Packet<HUB75_UNIT>;
}
