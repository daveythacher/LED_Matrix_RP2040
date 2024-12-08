/* 
 * File:   Matrix.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef MATRIX_H
#define MATRIX_H

#include "Serial/config.h"

namespace Matrix {
    class Matrix {
        public:
            static Matrix *get_matrix();

            virtual void show(Serial::packet *buffer, bool isBuffer = false) = 0;   // Future: No FPS limiter
        
        protected:
            Matrix();
        
        private:
            static Matrix *ptr;
    };
}

#endif

