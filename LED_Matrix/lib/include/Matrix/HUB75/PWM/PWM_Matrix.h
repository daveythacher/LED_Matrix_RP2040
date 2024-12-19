/* 
 * File:   PWM_Matrix.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef MATRIX_PWM_MATRIX_H
#define MATRIX_PWM_MATRIX_H

#include "Matrix/Matrix.h"
#include "Matrix/HUB75/PWM/PWM_Worker.h"

namespace Matrix {
    template <typename T, typename R> class PWM_Matrix : public Matrix<T, R> {
        public:
            ~PWM_Matrix();

            static PWM_Matrix *create_matrix(uint8_t scan, uint8_t columns);

            void show(Buffer<T> *buffer);
            void show(Packet<R> *buffer);
            unique_ptr<Buffer<T>> get_buffer();
            unique_ptr<Packet<R>> get_packet();

        private:
            PWM_Matrix();
            PWM_Matrix(uint8_t scan, uint8_t columns);

            uint8_t _scan;
            uint8_t _columns;
            PWM_Worker<T, R> *_worker;
    };
}

#endif
