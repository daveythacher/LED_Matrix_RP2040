/* 
 * File:   Matrix.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "Matrix/Matrix.h"
// TODO: #include "Matrix/HUB75/BCM/BCM_Matrix.h"
#include "Matrix/HUB75/PWM/PWM_Matrix.h"

namespace Matrix {
    template <typename T, typename R> Matrix<T, R> *Matrix<T, R>::ptr = nullptr;

    template <typename T, typename R> Matrix<T, R>::~Matrix() {}

    template <typename T, typename R> Matrix<T, R> *Matrix<T, R>::get_matrix() {
        if (ptr == nullptr) {
            // TODO:
            // Figure out the Matrix Family (HUB75)
            //  For HUB75, if Refresh <= 250 use BCM, PWM otherwise
        }

        return ptr;
    }

    template class Matrix<RGB24, uint8_t>;
    template class Matrix<RGB48, uint8_t>;
    template class Matrix<RGB_222, uint8_t>;
    template class Matrix<RGB_555, uint8_t>;
}