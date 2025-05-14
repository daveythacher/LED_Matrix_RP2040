/* 
 * File:   PWM_Packet.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef MATRIX_PWM_PACKET_H
#define MATRIX_PWM_PACKET_H

#include "Matrix/Packet.h"

namespace Matrix {
    // Use unique pointer due to get_line
    class PWM_Packet : public Packet {
        public:
            PWM_Packet();
            ~PWM_Packet();

            void set(uint8_t multiplex, uint8_t column, bool value);
            bool get(uint8_t multiplex, uint8_t column);

            uint8_t num_scan();
            uint16_t num_columns();
            uint32_t num_steps();
            
            uint8_t *get_line(uint8_t multiplex, uint16_t index);
            uint16_t get_line_length();

        private:

            uint8_t _scan;
            uint8_t _columns;
            uint16_t _steps;
            uint8_t *_buffer;
            bool *_table;
    };
}

#endif
