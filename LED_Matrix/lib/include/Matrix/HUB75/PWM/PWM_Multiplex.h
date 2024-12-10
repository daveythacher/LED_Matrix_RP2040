/* 
 * File:   PWM_Multiplex.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef MATRIX_PWM_MULTIPLEX_H
#define MATRIX_PWM_MULTIPLEX_H

#include "Matrix/Matrix.h"
#include "Matrix/HUB75/PWM/PWM_Buffer.h"
#include "Matrix/HUB75/PWM/PWM_Worker.h"

namespace Matrix {
    class PWM_Multiplex {
        public:
            PWM_Multiplex();
            ~PWM_Multiplex();

            void show(PWM_Buffer *buffer);

        private:
            void work(void *args);
            void send_buffer();

            PWM_Buffer *bufs[2];
            Concurrent::Thread *thread;
            Concurrent::Queue<uint8_t **> *queue;
    };
}

#endif
