/* 
 * File:   BCM_worker.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef BCM_WORKER_H
#define BCM_WORKER_H

#include <stdint.h>
#include <Serial/config.h>

namespace Matrix::Worker {
    template <typename T> struct BCM_worker {
        public:
            BCM_worker();
            void process_packet(Serial::packet *p);

        private:
            void build_index_table();
            T *get_table(uint16_t v, uint8_t i, uint8_t nibble);
            void set_pixel(uint8_t x, uint8_t y, uint16_t r0, uint16_t g0, uint16_t b0, uint16_t r1, uint16_t g1, uint16_t b1);

            union index_table_t {
                T table[16][6][4 / sizeof(T)];
                uint32_t v[16 * 6];
            };
            
            index_table_t index_table;
    };
}

#endif
