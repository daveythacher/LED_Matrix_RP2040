/* 
 * File:   Programs.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef MATRIX_BUS8_PWM_PROGRAMS_H
#define MATRIX_BUS8_PWM_PROGRAMS_H

#include <stdint.h>
#include "Matrix/BUS8/hw_config.h"

namespace Matrix::BUS8::PWM {
    class Programs {
        public:
            static uint8_t get_pmp_program(uint16_t *instructions, uint8_t len);
            static uint8_t get_ghost_program(uint16_t *instructions, uint8_t len);

            static const uint8_t WAKE_PMP = 4;
            static const uint8_t WAKE_GHOST = 5;
            static const uint8_t WAKE_MULTIPLEX = 6;

            struct Ghost_Packet {
                const uint8_t scan = MULTIPLEX;
                const uint8_t delay = (125 / (2 * BLANK_TIME)) + 1;
            };
    };
}

#endif
