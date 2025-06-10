/* 
 * File:   Multiplex.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef MATRIX_BUS8_PWM_MULTIPLEX_H
#define MATRIX_BUS8_PWM_MULTIPLEX_H

#include "Matrix/Matrix.h"
#include "Matrix/BUS8/PWM/Packet.h"
#include "Matrix/BUS8/PWM/Programs.h"
#include "Matrix/BUS8/hw_config.h"
#include "System/IPC.h"

namespace Matrix::BUS8::PWM {
    class Multiplex {
        public:
            Multiplex();

            void show(Packet *buffer);
            void work();

            void enable_power(bool shutdown);

        private:
            void send_buffer();
            void load_buffer(Packet *p);


            static const uint8_t num_buffers = 3;

            int dma_chan[4];
            Programs::Ghost_Packet ghost_packet;
            struct {uint32_t len; uint16_t *data;} address_table[3][(MULTIPLEX * (STEPS + 2)) + 1];
            uint16_t null_table[COLUMNS + 1];
            uint8_t counter;
            uint8_t bank;
            Packet *packets[num_buffers];
            uint16_t header;
            ::System::IPC<Packet *> *pipe[2];
    };
}

#endif
