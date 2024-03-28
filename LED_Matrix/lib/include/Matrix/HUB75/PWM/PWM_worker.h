/* 
 * File:   PWM_worker.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef PWM_WORKER_H
#define PWM_WORKER_H

#include <stdint.h>
#include "Serial/config.h"

namespace Matrix::Worker {
    template <typename T> struct PWM_worker {
        public:
            PWM_worker();
            void process_packet(Serial::packet *p);

        private:
            void build_index_table();
            T *get_table(uint16_t v, uint8_t i);
            void set_pixel(uint8_t x, uint8_t y, uint16_t r0, uint16_t g0, uint16_t b0, uint16_t r1, uint16_t g1, uint16_t b1);

            union index_table_t {
                T table[1 << PWM_bits][6][(1 << PWM_bits) / sizeof(T)];
                uint32_t v[(1 << PWM_bits) * 6 * (1 << PWM_bits) / 4];
            };
            
            index_table_t index_table;
    };
}

#endif
