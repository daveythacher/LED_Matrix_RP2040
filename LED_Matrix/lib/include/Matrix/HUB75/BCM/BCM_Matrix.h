/* 
 * File:   BCM_Matrix.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef MATRIX_BCM_MATRIX_H
#define MATRIX_BCM_MATRIX_H

#include "Matrix/Matrix.h"
#include "Matrix/HUB75/BCM/BCM_Buffer.h"
#include "Matrix/HUB75/BCM/BCM_Worker.h"

namespace Matrix {
    template <typename T> class BCM_Matrix : public Matrix {
        public:
            BCM_Matrix();

            void show(Serial::Protocol::Packet *buffer);
            void show(Packet *buffer);

        private:
            BCM_Buffer buf[Serial::num_framebuffers];
            BCM_Worker<T> worker;
            BCM_Buffer *buffer;
            uint8_t bank;
            uint8_t bank_vsync;
            bool vsync;

            uint32_t multiplex_stack[1024];
            uint32_t worker_stack[1024];
    };
}

#endif