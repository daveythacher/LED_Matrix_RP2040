/* 
 * File:   Programs.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef BCM_PROGRAMS_H
#define BCM_PROGRAMS_H

#include <stdint.h>

namespace Matrix {
    class Programs {
        public:
            static uint8_t get_pmp_program(uint16_t *instructions, uint8_t len);
            static uint8_t get_ghost_program(uint16_t *instructions, uint8_t len);
            static uint8_t get_address_program(uint16_t *instructions, uint8_t len);

            static const uint8_t WAKE_PMP = 4;
            static const uint8_t WAKE_GHOST = 5;
            static const uint8_t WAKE_MULTIPLEX = 6;
    };
}

#endif