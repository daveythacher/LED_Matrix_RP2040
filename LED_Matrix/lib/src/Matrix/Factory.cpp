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
            // TODO: Figure out the Matrix Family (HUB75)
            ptr = PWM_Matrix<RGBRGB, HUB75_UNIT, SIMD::SIMD<HUB75_UNIT, SIMD::SIMD_UNIT>>::create_matrix(8, 12, 8);
        }

        return ptr;
    }
}