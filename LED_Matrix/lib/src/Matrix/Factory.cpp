/* 
 * File:   Factory.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "Matrix/Factory.h"
#include "Matrix/Mono/PWM/PWM_Matrix.h"

namespace Matrix {
    Matrix *Factory::ptr = nullptr;

    Matrix *Factory::get_matrix() {
        if (ptr == nullptr) {
            ptr = PWM_Matrix::create_matrix();
        }

        return ptr;
    }
}