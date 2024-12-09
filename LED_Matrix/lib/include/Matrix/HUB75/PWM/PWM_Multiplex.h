/* 
 * File:   PWM_Matrix.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef MATRIX_PWM_MATRIX_H
#define MATRIX_PWM_MATRIX_H

#include "Matrix/Matrix.h"
#include "Matrix/HUB75/PWM/PWM_Buffer.h"
#include "Matrix/HUB75/PWM/PWM_Worker.h"

namespace Matrix {
    template <typename T> class PWM_Multiplex {
        public:
            PWM_Multiplex();

        private:
            void send_buffer();
            PWM_Buffer *get_front_buffer();

            PWM_Buffer buf[Serial::num_framebuffers];
            PWM_Worker<T> worker;
            PWM_Buffer *buffer;
            uint8_t bank;
            uint8_t bank_vsync;
            bool vsync;

            uint32_t multiplex_stack[1024];
            uint32_t worker_stack[1024];
    };
}

#endif
