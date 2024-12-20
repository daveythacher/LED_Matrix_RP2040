/* 
 * File:   Factory.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef MATRIX_FACTORY_H
#define MATRIX_FACTORY_H

#include "Matrix/Matrix.h"

namespace Matrix {
    template <typename T, typename R> class Factory {
        public:
            // Future: Configuration Structure?
            //  Hardware definitions
            //  Remote init?
            static Matrix<T, R> *get_matrix();
        
        private:
            static Matrix<T, R> *ptr;
    };
}

#endif
