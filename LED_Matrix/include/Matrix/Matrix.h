/* 
 * File:   Matrix.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef MATRIX_H
#define MATRIX_H

#include <new>
#include "Serial/Protocol/serial.h"

namespace Matrix {
    class Packet {
        public:
            uint8_t *serialize();
            void deserialize(uint8_t *buf, uint16_t len);

            static Packet *create() {
                return new(std::align_val_t(sizeof(uint32_t))) Packet();
            }

        private:
            alignas(uint32_t) uint8_t buffer[16 * 1024];
    };

    class Matrix {
        public:
            // Future: Configuration Structure?
            static Matrix *get_matrix();

            virtual void show(Serial::Protocol::Packet *buffer) = 0;   // Future: No FPS limiter
            virtual void show(Packet *buffer) = 0;                      // Future: No FPS limiter
        
        protected:
            Matrix();
        
        private:
            static Matrix *ptr;
    };
}

#endif

