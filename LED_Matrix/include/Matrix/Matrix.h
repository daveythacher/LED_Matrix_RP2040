/* 
 * File:   Matrix.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef MATRIX_H
#define MATRIX_H

#include <memory>
#include "Matrix/Packet.h"
#include "Matrix/Types.h"
using std::unique_ptr;

namespace Matrix {
    template <typename T, typename R> class Matrix {
        public:
            virtual void show(unique_ptr<Packet<R>> &packet) = 0;       // Future: No FPS limiter
            virtual unique_ptr<Packet<R>> get_packet() = 0;
    };
}

#endif
