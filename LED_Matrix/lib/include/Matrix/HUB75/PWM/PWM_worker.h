/* 
 * File:   PWM_worker.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef PWM_WORKER_H
#define PWM_WORKER_H

#include <algorithm>
#include <stdint.h>
#include "Serial/config.h"
#include "SIMD/SIMD_QUARTER.h"

namespace Matrix::Worker {
    template <typename T> struct PWM_worker {
        public:
            PWM_worker();
            void process_packet(Serial::packet *p);
            void save_buffer(Matrix::Buffer *p);
            void save_buffer(Serial::packet *p);

        private:
            void build_index_table();
            SIMD::SIMD_QUARTER<T> *get_table(uint16_t v, uint8_t i);
            void set_pixel(uint8_t x, uint8_t y, uint16_t r0, uint16_t g0, uint16_t b0, uint16_t r1, uint16_t g1, uint16_t b1);

            constexpr static uint32_t size = std::max(((1 << PWM_bits) / SIMD::SIMD_QUARTER<T>::size()), (uint32_t) 1);
            SIMD::SIMD_QUARTER<T> index_table[1 << PWM_bits][6][size];
    };
}

#endif
