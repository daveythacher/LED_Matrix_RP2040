/* 
 * File:   PWM_Dot_Correct.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "Matrix/HUB75/PWM/PWM_Dot_Correct.h"

namespace Matrix {
    PWM_Dot_Correct *PWM_Dot_Correct::_ptr = nullptr;

    PWM_Dot_Correct::PWM_Dot_Correct() {
        // Do nothing
    }

    PWM_Dot_Correct::PWM_Dot_Correct(uint8_t scan, uint16_t steps, uint8_t comlumns) {
        _scan = scan;
        _steps = steps;
        _columns = comlumns;
        _table = new uint16_t[_scan * _columns];
    }

    PWM_Dot_Correct *PWM_Dot_Correct::create_pwm_dot_correct(uint8_t scan, uint16_t steps, uint8_t columns) {
        if (_ptr == nullptr) {
            _ptr = new PWM_Dot_Correct(scan, steps, columns);
        }

        return _ptr;
    }

    void PWM_Dot_Correct::set(uint8_t scan, uint8_t column, uint16_t value) {
        _table[(scan * _columns) + column] = value;
    }

    uint16_t PWM_Dot_Correct::get(uint8_t scan, uint8_t column) {
        return _table[(scan * _columns) + column];
    }
}
