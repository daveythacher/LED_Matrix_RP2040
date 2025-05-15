/* 
 * File:   PWM_Dot_Correct.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef MATRIX_PWM_DOT_CORRECT_H
#define MATRIX_PWM_DOT_CORRECT_H

#include <stdint.h>

namespace Matrix {
    class PWM_Dot_Correct {
        public:
            static PWM_Dot_Correct *create_pwm_dot_correct(uint8_t scan, uint16_t steps, uint8_t columns);

            void set(uint8_t scan, uint8_t column, uint16_t value);
            uint16_t get(uint8_t scan, uint8_t column);
        
        protected:
            PWM_Dot_Correct();
            PWM_Dot_Correct(uint8_t scan, uint16_t steps, uint8_t columns);

        private:
            uint8_t _scan;
            uint16_t _steps;
            uint8_t _columns;
            uint16_t *_table;
            
            static PWM_Dot_Correct *_ptr;
    };
}

#endif
