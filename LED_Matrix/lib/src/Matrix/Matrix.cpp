/* 
 * File:   Matrix.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "Matrix/Matrix.h"
#include "Matrix/HUB75/BCM/BCM_Matrix.h"
#include "Matrix/HUB75/PWM/PWM_Matrix.h"

namespace Matrix {
    Matrix *Matrix::ptr = nullptr;

    Matrix *Matrix::get_matrix() {
        if (ptr == nullptr) {
            // TODO:
        }

        return ptr;
    }
}