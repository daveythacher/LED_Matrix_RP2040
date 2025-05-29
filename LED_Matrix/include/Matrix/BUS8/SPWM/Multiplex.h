/* 
 * File:   Multiplex.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef MATRIX_BUS8_SPWM_MULTIPLEX_H
#define MATRIX_BUS8_SPWM_MULTIPLEX_H

#include "Matrix/Matrix.h"
#include "Matrix/BUS8/SPWM/Payload.h"
#include "Matrix/BUS8/SPWM/Programs.h"
#include "Matrix/BUS8/hw_config.h"

namespace Matrix::BUS8::SPWM {
    class Multiplex {
        public:
            Multiplex();

            void show(Payload *buffer);
            void work();

        private:
            void send_buffer();
            void load_buffer(Payload *p);


            static const uint8_t num_buffers = 3;

            int dma_chan[4];
            Programs::Ghost_Packet ghost_packet;
            struct {uint32_t len; uint16_t *data;} address_table[3][(MULTIPLEX * (STEPS + 2)) + 1];
            uint16_t null_table[COLUMNS + 1];
            uint8_t counter;
            uint8_t bank;
            Payload *packets[num_buffers];
            uint16_t header;
    };
}

#endif
