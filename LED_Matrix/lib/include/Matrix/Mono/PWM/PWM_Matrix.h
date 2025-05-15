/* 
 * File:   PWM_Matrix.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef MATRIX_PWM_MATRIX_H
#define MATRIX_PWM_MATRIX_H

#include "Matrix/Matrix.h"
#include "Matrix/Mono/PWM/PWM_Worker/PWM_Worker.h"

namespace Matrix {
    class PWM_Matrix : public Matrix {
        public:
            ~PWM_Matrix();

            static PWM_Matrix *create_matrix();

            void show(unique_ptr<Packet> &packet);
            unique_ptr<Packet> get_packet();

        protected:
            PWM_Matrix();

        private:
            PWM_Worker *_worker;
            
            static PWM_Matrix *ptr;
    };
}

#endif
