/* 
 * File:   Factory.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "Matrix/Factory.h"
// TODO: #include "Matrix/HUB75/BCM/BCM_Matrix.h"
#include "Matrix/HUB75/PWM/PWM_Matrix.h"

namespace Matrix {
    template <typename T, typename R> Matrix<T, R> *Factory<T, R>::ptr = nullptr;

    template <typename T, typename R> Matrix<T, R> *Factory<T, R>::get_matrix() {
        if (ptr == nullptr) {
            // TODO:
            // Figure out the Matrix Family (HUB75)
            //  For HUB75, if Refresh <= 250 use BCM, PWM otherwise
        }

        return ptr;
    }
}