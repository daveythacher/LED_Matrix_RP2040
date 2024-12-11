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
    class PWM_Matrix : public Matrix {
        public:
            PWM_Matrix();
            ~PWM_Matrix();

            void show(Buffer *buffer);
            void show(Packet *buffer);

        private:
            PWM_Worker *worker;
    };
}

#endif
