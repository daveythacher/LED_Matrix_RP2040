/* 
 * File:   Buffer.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "Matrix/Buffer.h"
#include "Matrix/Types.h"

namespace Matrix {
    template <typename T> Buffer<T>::Buffer() {}

    template <typename T> Buffer<T>::Buffer(uint8_t scan, uint8_t columns) {
        _scan = scan;
        _columns = columns;
        _buffer = new T[scan * columns];
    }

    template <typename T> Buffer<T>::~Buffer() {
        delete _buffer;
    }

    template <typename T> Buffer<T> *Buffer<T>::create_buffer(uint8_t scan, uint8_t columns) {
        return new Buffer<T>(scan, columns);
    }

    template <typename T> void Buffer<T>::set(uint8_t x, uint8_t y, T *v) {
        // TODO:
    }
    
    template <typename T> void Buffer<T>::ntoh() {
        // TODO:
    }

    template class Buffer<RGB24>;
    template class Buffer<RGB48>;
    template class Buffer<RGB_222>;
    template class Buffer<RGB_555>;
}