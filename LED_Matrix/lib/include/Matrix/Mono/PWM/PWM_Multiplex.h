/* 
 * File:   PWM_Multiplex.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef MATRIX_PWM_MULTIPLEX_H
#define MATRIX_PWM_MULTIPLEX_H

#include "Matrix/Matrix.h"
#include "Matrix/Mono/PWM/PWM_Packet.h"
#include "Matrix/Mono/PWM/PWM_Programs.h"

namespace Matrix {
    class PWM_Multiplex {
        public:
            PWM_Multiplex();
            ~PWM_Multiplex();

            void show(PWM_Packet *buffer);

        private:
            void work(void *args);
            void send_buffer();


            int dma_chan[4];
            //PWM_Buffer *bufs[2];
            Concurrent::Thread *thread;
            Concurrent::Queue<uint8_t **> *queue;
            PWM_Programs::Ghost_Packet ghost_packet;
            struct {uint32_t len; uint8_t *data;} address_table[3][(MULTIPLEX * ((1 << PWM_bits) + 2)) + 1];
            uint8_t null_table[COLUMNS + 1];
            uint8_t header;
    };
}

#endif
