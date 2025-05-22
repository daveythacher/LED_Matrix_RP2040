/* 
 * File:   Matrix.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef MATRIX_BUS8_PWM_MATRIX_H
#define MATRIX_BUS8_PWM_MATRIX_H

#include "Matrix/Matrix.h"
#include "Matrix/BUS8/PWM/Multiplex.h"

namespace Matrix::BUS8::PWM {
    class Matrix : public Matrix::Matrix {
        public:
            static Matrix *create_matrix();

            void show(unique_ptr<Packet> &packet);
            unique_ptr<Packet> get_packet();

        private:
            Matrix();

            static Matrix *ptr;

            Multiplex *multiplex;
    };
}

#endif
