/* 
 * File:   Factory.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "Matrix/Factory.h"
#include "Matrix/HUB75/PWM/PWM_Matrix.h"

// TODO: Figure this out
#include "Matrix/HUB75/PWM/PWM_Multiplex.h"
#include "Matrix/HUB75/hw_config.h"
#include "SIMD/SIMD.h"

namespace Matrix {
    template <typename T, typename R> Matrix<T, R> *Factory<T, R>::ptr = nullptr;

    template <typename T, typename R> Matrix<T, R> *Factory<T, R>::get_matrix() {
        if (ptr == nullptr) {
            // TODO:
            // Figure out the Matrix Family (HUB75)
            //  For HUB75, if Refresh <= 250 use BCM, PWM otherwise
            ptr = PWM_Matrix<RGB24, HUB75_UNIT, SIMD::SIMD<HUB75_UNIT, SIMD::SIMD_UNIT>>::create_matrix(8, 4, 32);  // TODO: Fix this
        }

        return ptr;
    }
}