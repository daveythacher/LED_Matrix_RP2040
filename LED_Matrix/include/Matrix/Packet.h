/* 
 * File:   Packet.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef MATRIX_PACKET_H
#define MATRIX_PACKET_H

#include <stdint.h>

namespace Matrix {
    class Packet {
        public:
            virtual ~Packet() {}

            virtual void set(uint8_t scan, uint16_t steps, uint8_t column, uint8_t val) = 0;
            virtual uint8_t get(uint8_t scan, uint16_t steps, uint8_t column) = 0;
            
            virtual uint8_t num_scan() = 0;
            virtual uint8_t num_columns() = 0;
            virtual uint16_t num_steps() = 0;
    };
}

#endif

