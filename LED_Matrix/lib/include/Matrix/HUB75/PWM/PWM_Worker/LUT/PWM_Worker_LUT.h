/* 
 * File:   PWM_Worker_LUT.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef MATRIX_PWM_WORKER_LUT_H
#define MATRIX_PWM_WORKER_LUT_H

#include <stdint.h>
#include "Matrix/Buffer.h"
#include "Matrix/Packet.h"
#include "Matrix/HUB75/PWM/PWM_Worker/PWM_Worker.h"
#include "Concurrent/Thread/Thread.h"
#include "Concurrent/Queue/Queue.h"
#include "Concurrent/Mutex/Mutex.h"

namespace Matrix {
    template <typename R> class PWM_Multiplex;

    template <typename T, typename R, typename W> class PWM_Worker_LUT : public PWM_Worker {
        public:
            PWM_Worker_LUT(uint8_t scan, uint16_t steps, uint8_t columns);
            ~PWM_Worker_LUT();

            void convert(Packet<R> *packet);
            void convert(Buffer<T> *buffer);

        private:
            static void work(void *args);
            void build_index_table();
            W *get_table(uint16_t v, uint8_t i);
            void set_pixel(R *val, T *pixel, uint8_t index, uint8_t shift);
            static uint8_t get_thread_id();

            uint8_t _scan;
            uint16_t _steps;
            uint8_t _columns;
            uint16_t _size;
            uint8_t _width;
            volatile bool _idle;
            W *_index_table;
            PWM_Multiplex<R> *_multiplex;
            Concurrent::Thread *_thread[2];
            Concurrent::Queue<uint8_t **> *_queue;
            Concurrent::Mutex *_mutex;
    };
}

#endif
