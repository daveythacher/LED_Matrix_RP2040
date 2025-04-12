/* 
 * File:   PWM_Dot_Correct.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef MATRIX_PWM_DOT_CORRECT_H
#define MATRIX_PWM_DOT_CORRECT_H

namespace Matrix {
    class PWM_Dot_Correct {
        public:
            ~PWM_Buffer();

            static PWM_Buffer *create_pwm_buffer(uint8_t rows, uint8_t columns);

            void set(uint8_t row, uint8_t column, uint8_t color, uint8_t value);
            T get(uint8_t row, uint8_t column);

            uint8_t num_rows();
            uint8_t num_columns();

        private:
            PWM_Buffer();
            PWM_Buffer(uint8_t rows, uint8_t columns);

            uint8_t _rows;
            uint8_t _columns;
            uint8_t ***_table;
    };
}

#endif
