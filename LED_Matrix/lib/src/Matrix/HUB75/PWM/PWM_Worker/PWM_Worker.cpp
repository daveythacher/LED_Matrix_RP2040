/* 
 * File:   PWM_Worker.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#include "Matrix/HUB75/PWM/PWM_Worker/PWM_Worker.h"
#include "Matrix/HUB75/PWM/PWM_Worker/LUT/PWM_Worker_LUT.h"
#include "Matrix/HUB75/PWM/PWM_Multiplex.h"
#include "Matrix/HUB75/hw_config.h"
#include "SIMD/SIMD_QUARTER.h"
#include "SIMD/SIMD_SINGLE.h"

namespace Matrix {
    template <typename T, typename R, typename W> PWM_Worker<T, R, W> *PWM_Worker<T, R, W>::get_worker(uint8_t scan, uint16_t steps, uint8_t columns) {
        // TODO: Make this configurable
        return new PWM_Worker_LUT<T, R, W>(scan, steps, columns);
    }

    template class PWM_Worker<RGB24, HUB75_UNIT, SIMD::SIMD_UNIT<HUB75_UNIT>>;
    template class PWM_Worker<RGB48, HUB75_UNIT, SIMD::SIMD_UNIT<HUB75_UNIT>>;
    template class PWM_Worker<RGB_222, HUB75_UNIT, SIMD::SIMD_UNIT<HUB75_UNIT>>;
    template class PWM_Worker<RGB_555, HUB75_UNIT, SIMD::SIMD_UNIT<HUB75_UNIT>>;
}
