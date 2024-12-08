/* 
 * File:   Matrix.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "Matrix/Matrix.h"

namespace Matrix {
    Matrix *Matrix::ptr = nullptr;

    Matrix *Matrix::get_matrix() {
        if (ptr == nullptr) {
            // TODO:
        }

        return ptr;
    }
}