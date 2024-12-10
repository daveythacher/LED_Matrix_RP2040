/* 
 * File:   PWM_Matrix.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "Matrix/HUB75/PWM/PWM_Matrix.h"

namespace Matrix {
    PWM_Matrix::PWM_Matrix() {
        worker = new PWM_Worker();
    }

    PWM_Matrix::~PWM_Matrix() {
        delete worker;
    }

    void PWM_Matrix::show(Serial::Protocol::Packet *buffer) {
        worker->convert(buffer, true);
    }
    
    void PWM_Matrix::show(Packet *buffer) {
        worker->convert(buffer, true);
    }
}
