/* 
 * File:   BCM_Worker.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef MATRIX_BCM_WORKER_H
#define MATRIX_BCM_WORKER_H

#include "Matrix/HUB75/BCM/BCM_Buffer.h"
#include "Serial/config.h"

namespace Matrix {
    template <typename T> class BCM_Worker {
        public:
            BCM_Worker();
            void process_packet(BCM_Buffer *buffer, Serial::packet *p);
            void save_buffer(BCM_Buffer *buffer, BCM_Buffer *p);
            void save_buffer(BCM_Buffer *buffer, Serial::packet *p);

        private:
            void build_index_table();
            T *get_table(uint16_t v, uint8_t i, uint8_t nibble);
            void set_pixel(BCM_Buffer *buffer, uint8_t x, uint8_t y, uint16_t r0, uint16_t g0, uint16_t b0, uint16_t r1, uint16_t g1, uint16_t b1);

            union index_table_t {
                T table[16][6][4 / sizeof(T)];
                uint32_t v[16 * 6];
            };
            
            index_table_t index_table;
    };
}

#endif
