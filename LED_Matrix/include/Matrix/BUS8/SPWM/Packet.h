/* 
 * File:   Packet.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef MATRIX_BUS8_SPWM_PACKET_H
#define MATRIX_BUS8_SPWM_PACKET_H

#include "Matrix/Packet.h"

namespace Matrix::BUS8::SPWM {
    // Use unique pointer due to get_line
    // TODO: Add Dot Correct here
    class Packet : public ::Matrix::Packet {
        public:
            ~Packet();

            static Packet *create_packet(uint8_t scan, uint16_t steps, uint8_t columns);

            void set(uint8_t multiplex, uint16_t index, uint8_t column, uint8_t value);
            uint8_t get(uint8_t multiplex, uint16_t index, uint8_t column);

            uint8_t num_scan();
            uint8_t num_columns();
            uint16_t num_steps();

            // Future: Consider deserialize

        private:
            Packet();
            Packet(::Matrix::Packet *packet);
            Packet(uint8_t scan, uint16_t steps, uint8_t columns);

            // Future: Consider lock and unlock methods for use within copy constructor
            //  We own packet during that process and block all writes to packet.
            //  This can cause tearing, if not careful.

            uint8_t _scan;
            uint8_t _columns;
            uint16_t _steps;
            uint16_t *_buffer;
    };
}

#endif
