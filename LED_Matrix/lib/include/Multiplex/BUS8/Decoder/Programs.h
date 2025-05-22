/* 
 * File:   Programs.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef MULTIPLEX_BUS8_DECODER_PROGRAMS_H
#define MULTIPLEX_BUS8_DECODER_PROGRAMS_H

#include <stdint.h>

namespace Multiplex::BUS8::Decoder {
    class Programs {
        public:
            static uint8_t get_address_program(uint16_t *instructions, uint8_t len, int start_flag, int signal_flag);
    };
}

#endif
