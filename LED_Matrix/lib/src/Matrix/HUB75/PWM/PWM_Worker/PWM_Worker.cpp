/* 
 * File:   PWM_Worker.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#include "Matrix/HUB75/PWM/PWM_Worker/PWM_Worker.h"
#include "Matrix/HUB75/PWM/PWM_Worker/LUT/PWM_Worker_LUT.h"

namespace Matrix {
    PWM_Worker::~PWM_Worker() {
        // Do nothing
    }

    PWM_Worker *PWM_Worker::get_worker() {
        return new PWM_Worker_LUT();
    }
}
