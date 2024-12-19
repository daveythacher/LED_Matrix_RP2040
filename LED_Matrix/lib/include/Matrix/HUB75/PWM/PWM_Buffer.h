/* 
 * File:   PWM_Buffer.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef MATRIX_PWM_BUFFER_H
#define MATRIX_PWM_BUFFER_H

#include "Matrix/Packet.h"
#include "Serial/config.h"

namespace Matrix {
    // Use unique pointer due to get_line
    template <typename T> class PWM_Buffer : public Packet<T> {
        public:
            ~PWM_Buffer();

            static PWM_Buffer *create_pwm_buffer(uint8_t scan, uint16_t steps, uint8_t columns);

            void set(uint8_t multiplex, uint16_t index, uint8_t column, T value);
            T get(uint8_t multiplex, uint16_t index, uint8_t column);
            T *get_line(uint8_t multiplex, uint16_t index);
            uint16_t get_line_length();

        private:
            PWM_Buffer();
            PWM_Buffer(uint8_t scan, uint16_t steps, uint8_t columns);

            uint8_t _scan;
            uint8_t _columns;
            uint16_t _steps;
            T *_buffer;
    };
}

#endif
