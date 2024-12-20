/* 
 * File:   Packet.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef MATRIX_PACKET_H
#define MATRIX_PACKET_H

#include <stdint.h>

namespace Matrix {
    // These are used for raw bit data.
    template <typename T> class Packet {
        public:
            virtual void set(uint8_t scan, uint16_t step, uint8_t column, T val) = 0;
            virtual T get(uint8_t scan, uint16_t step, uint8_t column) = 0;
            virtual uint8_t num_scan() = 0;
            virtual uint16_t num_columns(bool isRaw = false) = 0;
            virtual uint32_t num_steps() = 0;
    };
}

#endif

