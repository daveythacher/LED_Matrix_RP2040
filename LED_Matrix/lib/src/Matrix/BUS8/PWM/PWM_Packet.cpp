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
    PWM_Packet::PWM_Packet() {}

    PWM_Packet::PWM_Packet(uint8_t scan, uint16_t steps, uint8_t columns) {
        _scan = scan;
        _columns = columns;
        _steps = steps;
        _buffer = new uint8_t[scan * steps * (columns + 1)];

        memset(_buffer, columns - 1, scan * steps * columns);
    }

    PWM_Packet::~PWM_Packet() {
        delete[] _buffer;
    }

    PWM_Packet *PWM_Packet::create_pwm_packet(uint8_t scan, uint16_t steps, uint8_t columns) {
        return new PWM_Packet(scan, steps, columns);
    }

    void PWM_Packet::set(uint8_t multiplex, uint16_t index, uint8_t column, uint8_t value) {
        if (multiplex > _scan || index > _steps || column > _columns)
            return;

        uint32_t i = multiplex * _steps * (_columns + 1);
        i += index * (_columns + 1);
        i += column;

        _buffer[i + 1] = value;
    }

    uint8_t PWM_Packet::get(uint8_t multiplex, uint16_t index, uint8_t column) {
        if (multiplex > _scan || index > _steps || column > _columns)
            return _buffer[1];

        uint32_t i = multiplex * _steps * (_columns + 1);
        i += index * (_columns + 1);
        i += column;

        return _buffer[i + 1];
    }

    uint8_t PWM_Packet::num_scan() {
        return _scan;
    }

    uint8_t PWM_Packet::num_columns() {
        return _columns;
    }

    uint16_t PWM_Packet::num_steps() {
        return _steps;
    }

    uint8_t *PWM_Packet::get_line(uint8_t multiplex, uint16_t index) {
        if (multiplex > _scan || index > _steps)
            return nullptr;

        uint32_t i = multiplex * _steps * (_columns + 1);
        i += index * (_columns + 1);

        return &_buffer[i];
    }

    uint16_t PWM_Packet::get_line_length() {
        return _columns + 1;
    }
}
