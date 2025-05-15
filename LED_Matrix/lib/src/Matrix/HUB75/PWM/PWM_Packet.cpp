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
        // Do nothing
    }

    PWM_Packet::PWM_Packet(uint8_t scan, uint16_t steps, uint8_t columns) {
        _scan = scan;
        _columns = columns;
        _buffer = new uint16_t[_scan * (_columns + 1)];
        _table = new bool[_scan * _columns];
        _correction = PWM_Dot_Correct::create_pwm_dot_correct(_scan, steps, columns);

        memset(_buffer, _columns - 1, _scan * _columns);
        memset(_table, false, _scan * _columns);
    }

    PWM_Packet::~PWM_Packet() {
        delete[] _buffer;
    }

    PWM_Packet *PWM_Packet::create_packet(uint8_t scan, uint16_t steps, uint8_t columns) {
        return new PWM_Packet(scan, steps, columns);
    }

    void PWM_Packet::set(uint8_t multiplex, uint8_t column, bool value) {
        if (multiplex > _scan || column > _columns)
            return;

        uint32_t i = multiplex * (_columns + 1);
        i += column;

        _buffer[i + 1] = value ? _correction->get(multiplex, column) : 0;
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

    // TODO: Look into this
    uint16_t *PWM_Packet::get_line(uint8_t multiplex) {
        if (multiplex > _scan)
            return nullptr;

        return &_buffer[multiplex * (_columns + 1)];
    }

    // TODO: Look into this
    uint16_t PWM_Packet::get_line_length() {
        return _columns + 1;
    }
}
