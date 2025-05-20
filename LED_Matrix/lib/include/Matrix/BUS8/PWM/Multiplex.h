/* 
 * File:   Multiplex.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef MATRIX_PWM_MULTIPLEX_H
#define MATRIX_PWM_MULTIPLEX_H

#include "Matrix/Matrix.h"
#include "Matrix/BUS8/PWM/PWM_Packet.h"
#include "Matrix/BUS8/PWM/PWM_Programs.h"
#include "Matrix/BUS8/hw_config.h"
#include "Concurrent/Queue/Queue.h"
#include "Concurrent/Thread/Thread.h"

namespace Matrix {
    class Multiplex {
        public:
            PWM_Multiplex();

            void show(PWM_Packet *buffer);

        private:
            static void work(void *args);
            void send_buffer();
            void load_buffer(PWM_Packet *p);


            int dma_chan[4];
            Concurrent::Thread *thread;
            Concurrent::Queue<PWM_Packet *> *queue;
            PWM_Programs::Ghost_Packet ghost_packet;
            struct {uint32_t len; uint8_t *data;} address_table[3][(MULTIPLEX * (STEPS + 2)) + 1];
            uint8_t null_table[COLUMNS + 1];
            uint16_t header;
            uint8_t counter;
            uint8_t bank;
    };
}

#endif
