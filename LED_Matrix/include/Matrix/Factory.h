/* 
 * File:   Factory.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef MATRIX_FACTORY_H
#define MATRIX_FACTORY_H

#include "Matrix/Matrix.h"

namespace Matrix {
    class Factory {
        public:
            // Future: Configuration Structure?
            //  Hardware definitions
            //  Remote init?
            static Matrix *get_matrix();
        
        private:
            static Matrix *ptr;
    };
}

#endif
