/* 
 * File:   Matrix.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "Matrix/Matrix.h"
#include "Matrix/HUB75/BCM/BCM_Matrix.h"
#include "Matrix/HUB75/PWM/PWM_Matrix.h"

namespace Matrix {
    template <typename T> Matrix<T> *Matrix<T>::ptr = nullptr;

    template <typename T> Matrix<T>::Matrix() {}

    template <typename T> Matrix<T>::~Matrix() {}

    template <typename T> Matrix<T> *Matrix<T>::get_matrix() {
        if (ptr == nullptr) {
            // TODO:
        }

        return ptr;
    }

    template class Matrix<RGB24>;
    template class Matrix<RGB48>;
    template class Matrix<RGB_222>;
    template class Matrix<RGB_555>;
}