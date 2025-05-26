/* 
 * File:   Matrix.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef MATRIX_H
#define MATRIX_H

#include <memory>
#include "Matrix/Packet.h"
using std::unique_ptr;

namespace Matrix {
    class Matrix {
        public:
            static Matrix *get_matrix();

            virtual void show(unique_ptr<Packet> &packet) = 0;
            virtual unique_ptr<Packet> get_packet() = 0;
            virtual void work() = 0;
        
        protected:
            Matrix();
    };
}

#endif
