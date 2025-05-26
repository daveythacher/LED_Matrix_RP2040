/* 
 * File:   Packet.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include <string.h>
#include "Matrix/BUS8/PWM/Packet.h"
#include "Matrix/BUS8/hw_config.h"

// Every line starts with a counter variable indexed from zero instead of one

namespace Matrix::BUS8::PWM {
    Packet::Packet() {}

    Packet::Packet(uint8_t scan, uint16_t steps, uint8_t columns) {
        _scan = scan;
        _columns = columns;
        _steps = steps;
        _buffer = new uint16_t[scan * steps * (columns + 1)];

        memset(_buffer, columns - 1, scan * steps * columns);
    }

    Packet::~Packet() {
        delete[] _buffer;
    }

    Packet *Packet::create_packet(uint8_t scan, uint16_t steps, uint8_t columns) {
        return new Packet(scan, steps, columns);
    }

    void Packet::set(uint8_t multiplex, uint16_t index, uint8_t column, uint8_t value) {
        if (multiplex > _scan || index > _steps || column > _columns)
            return;

        uint32_t i = multiplex * _steps * (_columns + 1);
        i += index * (_columns + 1);
        i += column;

        _buffer[i + 1] = value;
    }

    uint8_t Packet::get(uint8_t multiplex, uint16_t index, uint8_t column) {
        if (multiplex > _scan || index > _steps || column > _columns)
            return _buffer[1];

        uint32_t i = multiplex * _steps * (_columns + 1);
        i += index * (_columns + 1);
        i += column;

        return _buffer[i + 1];
    }

    uint8_t Packet::num_scan() {
        return _scan;
    }

    uint8_t Packet::num_columns() {
        return _columns;
    }

    uint16_t Packet::num_steps() {
        return _steps;
    }

    uint16_t *Packet::get_line(uint8_t multiplex, uint16_t index) {
        if (multiplex > _scan || index > _steps)
            return nullptr;

        uint32_t i = multiplex * _steps * (_columns + 1);
        i += index * (_columns + 1);

        return &_buffer[i];
    }

    uint16_t Packet::get_line_length() {
        return _columns + 1;
    }
}
