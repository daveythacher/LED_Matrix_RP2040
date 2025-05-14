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
    PWM_Packet::PWM_Packet() {
        _scan = MULTIPLEX;
        _columns = COLUMNS;
        _steps = STEPS;
        _buffer = new uint8_t[_scan * _steps * (_columns + 1)];
        _table = new bool[_scan * _columns];

        memset(_buffer, _columns - 1, _scan * _steps * _columns);
        memset(_table, false, _scan * _columns);
    }

    PWM_Packet::~PWM_Packet() {
        delete[] _buffer;
    }

    // TODO: Add Dot Correct function here
    void PWM_Packet::set(uint8_t multiplex, uint8_t column, bool value) {
        if (multiplex > _scan || column > _columns)
            return;

        uint32_t i = multiplex * (_columns + 1);
        i += column;

        _buffer[i + 1] = value; // TODO
        _table[multiplex * _columns + column] = value;
    }

    bool PWM_Packet::get(uint8_t multiplex, uint8_t column) {
        if (multiplex > _scan || column > _columns)
            return false;

        return _table[(multiplex * _columns) + column];
    }


    uint8_t PWM_Packet::num_scan() {
        return _scan;
    }

    uint16_t PWM_Packet::num_columns() {
        return _columns;
    }

    uint32_t PWM_Packet::num_steps() {
        return _steps;
    }

    // TODO: Look into this
    T *PWM_Packet::get_line(uint8_t multiplex, uint16_t index) {
        if (multiplex > _scan || index > _steps)
            return nullptr;

        uint32_t i = multiplex * _steps * (_columns + 1);
        i += index * (_columns + 1);

        return &_buffer[i];
    }

    // TODO: Look into this
    uint16_t PWM_Packet::get_line_length() {
        return _columns + 1;
    }
}
