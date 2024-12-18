/* 
 * File:   Matrix.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef MATRIX_H
#define MATRIX_H

#include "Matrix/Packet.h"
#include "Matrix/Buffer.h"
#include "Matrix/Types.h"

namespace Matrix {
    template <typename T> class Matrix {
        public:
            virtual ~Matrix();

            // Future: Configuration Structure?
            //  Hardware definitions
            static Matrix<T> *get_matrix();

            virtual void show(Buffer<T> *buffer) = 0;           // Future: No FPS limiter
            virtual void show(Packet *buffer) = 0;              // Future: No FPS limiter
        
        protected:
            Matrix();
        
        private:
            static Matrix<T> *ptr;
    };
}

#endif
