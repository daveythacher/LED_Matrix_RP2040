/* 
 * File:   PWM_Worker.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#include "Matrix/Mono/PWM/PWM_Worker/PWM_Worker.h"
#include "Matrix/Mono/PWM/PWM_Worker/LUT/PWM_Worker_LUT.h"

namespace Matrix {
    PWM_Worker::~PWM_Worker() {
        // Do nothing
    }

    PWM_Worker *PWM_Worker::get_worker(uint8_t scan, uint16_t steps, uint8_t columns) {
        return new PWM_Worker_LUT(scan, steps, columns);
    }
}
