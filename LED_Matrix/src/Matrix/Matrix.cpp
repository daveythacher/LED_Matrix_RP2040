/* 
 * File:   Matrix.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "Matrix/Matrix.h"
#include "Matrix/BUS8/PWM/Matrix.h"
#include "Matrix/config.h"

namespace Matrix {
    Matrix::Matrix() {
        // Do nothing
    }

    Matrix *Matrix::get_matrix() {
        return HARDWARE::ALGORITHM::Matrix::create_matrix();
    }
}