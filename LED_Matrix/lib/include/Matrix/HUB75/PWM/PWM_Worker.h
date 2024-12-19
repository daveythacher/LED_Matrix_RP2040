/* 
 * File:   PWM_worker.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef PWM_WORKER_H
#define PWM_WORKER_H

#include <stdint.h>
#include "SIMD/SIMD_QUARTER.h"  // TODO: Template this
#include "Matrix/HUB75/PWM/PWM_Multiplex.h"
#include "Concurrent/Thread/Thread.h"
#include "Concurrent/Queue/Queue.h"
#include "Concurrent/Mutex/Mutex.h"

namespace Matrix {
    template <typename T, typename R> class PWM_Worker {
        public:
            PWM_Worker(uint8_t scan, uint16_t steps, uint8_t columns);
            ~PWM_Worker();

            void convert(Packet<R> *buffer);
            void convert(Buffer<T> *buffer);

        private:
            static void work(void *args);
            void build_index_table();
            SIMD::SIMD_QUARTER<R> *get_table(uint16_t v, uint8_t i);
            void set_pixel(uint8_t x, uint8_t y, uint16_t r0, uint16_t g0, uint16_t b0, uint16_t r1, uint16_t g1, uint16_t b1);

            uint8_t _scan;
            uint16_t _steps;
            uint8_t _columns;
            uint16_t _size;
            uint8_t _width;
            SIMD::SIMD_QUARTER<R> *_index_table;
            PWM_Multiplex *_multiplex;
            Concurrent::Thread *_thread;
            Concurrent::Queue<uint8_t **> *_queue;
            Concurrent::Mutex *_mutex;
            
    };
}

#endif
