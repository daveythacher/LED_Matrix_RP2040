/* 
 * File:   Payload.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include <string.h>
#include "Matrix/BUS8/SPWM/Payload.h"
#include "Matrix/BUS8/hw_config.h"

// Every line starts with a counter variable indexed from zero instead of one

namespace Matrix::BUS8::SPWM {
    Payload::Payload() {}

    // Deep copy
    Payload::Payload(::Matrix::Packet *packet) {
        _scan = packet->num_scan();
        _columns = packet->num_columns();
        _steps = (STEPS_MAJOR + 1) * STEPS_MINOR;
        _buffer = new uint16_t[_scan * _steps * (_columns + 1)];

        memset(_buffer, _columns - 1, _scan * _steps * (_columns + 1));

        for (int h = 0; h < STEPS_MINOR; h++) {
            for (int i = 0; i < packet->num_columns(); i++) {
                for (int j = 0; j < packet->num_scan(); j++) {
                    for (int k = 0; k < (STEPS_MAJOR + 1); k++) {
                        if (k == 0) {
                            set(j, h * (STEPS_MAJOR + 1), i, packet->get(j, STEPS_MAJOR + h, i));
                        }
                        else {
                            set(j, (h * (STEPS_MAJOR + 1)) + k, i, packet->get(j, k - 1, i));
                        }
                    }
                }
            }
        }
    }

    Payload::~Payload() {
        delete[] _buffer;
    }

    Payload *Payload::create_payload(::Matrix::Packet *packet, uint8_t scan, uint16_t steps, uint8_t columns) {
        if (packet != nullptr) {
            if (packet->num_columns() == columns && packet->num_scan() == scan && packet->num_steps() == (STEPS_MAJOR + STEPS_MINOR)) {
                return new Payload(packet);
            }
        }

        return nullptr;
    }

    void Payload::set(uint8_t multiplex, uint16_t index, uint8_t column, uint8_t value) {
        if (multiplex > _scan || index > _steps || column > _columns)
            return;

        uint32_t i = multiplex * _steps * (_columns + 1);
        i += index * (_columns + 1);
        i += column;

        _buffer[i + 1] = value;
    }

    uint8_t Payload::get(uint8_t multiplex, uint16_t index, uint8_t column) {
        if (multiplex > _scan || index > _steps || column > _columns)
            return _buffer[1];

        uint32_t i = multiplex * _steps * (_columns + 1);
        i += index * (_columns + 1);
        i += column;

        return _buffer[i + 1];
    }

    uint8_t Payload::num_scan() {
        return _scan;
    }

    uint8_t Payload::num_columns() {
        return _columns;
    }

    uint16_t Payload::num_steps() {
        return _steps;
    }

    uint16_t *Payload::get_line(uint8_t multiplex, uint16_t index) {
        if (multiplex > _scan || index > _steps)
            return nullptr;

        uint32_t i = multiplex * _steps * (_columns + 1);
        i += index * (_columns + 1);

        return &_buffer[i];
    }

    uint16_t Payload::get_line_length() {
        return _columns + 1;
    }
}
