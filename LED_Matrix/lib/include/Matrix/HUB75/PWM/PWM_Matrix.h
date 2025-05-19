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
    class PWM_Matrix : public Matrix {
        public:
            ~PWM_Matrix();

            static PWM_Matrix *create_matrix();

            void show(unique_ptr<Packet> &packet);
            unique_ptr<Packet> get_packet();

        private:
            PWM_Matrix();

            uint8_t _scan;
            uint8_t _pwm_bits;
            uint8_t _columns;
            PWM_Worker *_worker;
    };
}

#endif
