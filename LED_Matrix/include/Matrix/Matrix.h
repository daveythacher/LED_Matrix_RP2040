/* 
 * File:   Matrix.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef MATRIX_H
#define MATRIX_H

#include "Matrix/Packet.h"

namespace Matrix {
    class Matrix {
        public:
            static Matrix *get_matrix();

            virtual void show(Packet *packet) = 0;
            virtual Packet *get_packet() = 0;
            virtual void work() = 0;
        
        protected:
            Matrix();
    };
}

#endif
