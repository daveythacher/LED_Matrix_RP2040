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
#include "Concurrent/Queue/Queue.h"
#include "Concurrent/Thread/Thread.h"

namespace Matrix::BUS8::PWM {
    class Multiplex {
        public:
            Multiplex();

            void show(Packet *buffer);

        private:
            static void work(void *args);
            void send_buffer();
            void load_buffer(Packet *p);


            int dma_chan[4];
            Concurrent::Thread *thread;
            Concurrent::Queue<Packet *> *queue;
            Programs::Ghost_Packet ghost_packet;
            struct {uint32_t len; uint8_t *data;} address_table[3][(MULTIPLEX * (STEPS + 2)) + 1];
            uint8_t null_table[COLUMNS + 1];
            uint16_t header;
            uint8_t counter;
            uint8_t bank;
    };
}

#endif
