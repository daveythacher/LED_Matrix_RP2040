/* 
 * File:   Matrix.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef MATRIX_H
#define MATRIX_H

#include <memory>
#include "Matrix/Packet.h"
#include "Matrix/Buffer.h"
#include "Matrix/Types.h"
using std::unique_ptr;

namespace Matrix {
    template <typename T, typename R> class Matrix {
        public:
            virtual ~Matrix();

            // Future: Configuration Structure?
            //  Hardware definitions
            //  Remote init?
            static Matrix *get_matrix();

            virtual void show(unique_ptr<Buffer<T>> &buffer) = 0;       // Future: No FPS limiter
            virtual void show(unique_ptr<Packet<R>> &packet) = 0;       // Future: No FPS limiter
            virtual unique_ptr<Buffer<T>> get_buffer() = 0;
            virtual unique_ptr<Packet<R>> get_packet() = 0;
        
        private:
            static Matrix *ptr;
    };
}

#endif
