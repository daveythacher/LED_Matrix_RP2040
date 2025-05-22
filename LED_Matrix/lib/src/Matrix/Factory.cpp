/* 
 * File:   Factory.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "Matrix/Factory.h"
#include "Matrix/BUS8/PWM/Matrix.h"

namespace Matrix {
    Matrix *Factory::ptr = nullptr;

    Matrix *Factory::get_matrix() {
        if (ptr == nullptr) {
            ptr = BUS8::PWM::Matrix::create_matrix();   // TODO: Preprocessor macros ???
        }

        return ptr;
    }
}