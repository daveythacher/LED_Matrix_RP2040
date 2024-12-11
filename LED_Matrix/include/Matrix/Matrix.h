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
    class Matrix {
        public:
            // Future: Configuration Structure?
            //  Hardware definitions
            static Matrix *get_matrix();

            virtual void show(Buffer<RGB24> *buffer) = 0;       // Future: No FPS limiter
            virtual void show(Buffer<RGB48> *buffer) = 0;       // Future: No FPS limiter
            virtual void show(Buffer<RGB_222> *buffer) = 0;     // Future: No FPS limiter
            virtual void show(Buffer<RGB_555> *buffer) = 0;     // Future: No FPS limiter
            virtual void show(Packet *buffer) = 0;              // Future: No FPS limiter
        
        protected:
            Matrix();
        
        private:
            static Matrix *ptr;
    };
}

#endif

