/* 
 * File:   PWM_worker.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef PWM_WORKER_H
#define PWM_WORKER_H

#include <algorithm>
#include <stdint.h>
#include "Matrix/Matrix.h"
#include "Matrix/HUB75/PWM/PWM_Buffer.h"
#include "Matrix/HUB75/PWM/memory_format.h"
#include "SIMD/SIMD_QUARTER.h"
#include "Matrix/HUB75/PWM/PWM_Multiplex.h"
#include "Concurrent/Thread/Thread.h"
#include "Concurrent/Queue/Queue.h"

namespace Matrix {
    template <typename T, typename R> class PWM_Worker {
        public:
            PWM_Worker();
            ~PWM_Worker();

            void convert(Packet *buffer, bool release);
            void convert(Buffer<T> *buffer, bool release);

        private:
            void work(void *args);
            void build_index_table();
            SIMD::SIMD_QUARTER<uint8_t> *get_table(uint16_t v, uint8_t i);
            void set_pixel(uint8_t x, uint8_t y, uint16_t r0, uint16_t g0, uint16_t b0, uint16_t r1, uint16_t g1, uint16_t b1);

            constexpr static uint32_t size = std::max(((1 << PWM_bits) / SIMD::SIMD_QUARTER<uint8_t>::size()), (uint32_t) 1);
            SIMD::SIMD_QUARTER<uint8_t> index_table[1 << PWM_bits][6][size];
            PWM_Multiplex *multiplex;
            Concurrent::Thread *thread;
            Concurrent::Queue<uint8_t **> *queue;
            
    };
}

#endif
