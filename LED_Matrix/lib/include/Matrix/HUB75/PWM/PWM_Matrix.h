/* 
 * File:   PWM_Matrix.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef MATRIX_PWM_MATRIX_H
#define MATRIX_PWM_MATRIX_H

#include "Matrix/Matrix.h"
#include "Matrix/HUB75/PWM/PWM_Worker/PWM_Worker.h"

namespace Matrix {
    template <typename T, typename R, typename W> class PWM_Matrix : public Matrix<T, R> {
        public:
            ~PWM_Matrix();

            static PWM_Matrix *create_matrix(uint8_t scan, uint8_t pwm_bits, uint8_t columns);

            void show(unique_ptr<Packet<R>> &packet);
            unique_ptr<Packet<R>> get_packet();

        private:
            PWM_Matrix();
            PWM_Matrix(uint8_t scan, uint8_t pwm_bits, uint8_t columns);

            uint8_t _scan;
            uint8_t _pwm_bits;
            uint8_t _columns;
            PWM_Worker<T, R, W> *_worker;
    };
}

#endif
