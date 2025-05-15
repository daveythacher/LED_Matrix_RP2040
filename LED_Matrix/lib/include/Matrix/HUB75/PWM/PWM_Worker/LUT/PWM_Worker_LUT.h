/* 
 * File:   PWM_Worker_LUT.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef MATRIX_PWM_WORKER_LUT_H
#define MATRIX_PWM_WORKER_LUT_H

#include "Matrix/HUB75/PWM/PWM_Worker/PWM_Worker.h"
#include "Matrix/Types.h"
#include "Concurrent/Thread/Thread.h"
#include "Concurrent/Queue/Queue.h"
#include "Concurrent/Mutex/Mutex.h"

namespace Matrix {
    class PWM_Multiplex;

    class PWM_Worker_LUT : public PWM_Worker {
        public:
            PWM_Worker_LUT(uint8_t scan, uint16_t steps, uint8_t columns);
            ~PWM_Worker_LUT();

            void convert(Packet *packet);

        private:
            static void work(void *args);
            void build_index_table();
            uint8_t *get_table(uint16_t v, uint8_t i);
            void set_pixel(uint8_t *val, RGBRGB *pixel, uint8_t index, uint8_t shift);

            uint8_t _scan;
            uint16_t _steps;
            uint8_t _columns;
            uint16_t _size;
            uint8_t _width;
            volatile bool _idle;
            uint8_t *_index_table;
            PWM_Multiplex *_multiplex;
            Concurrent::Thread *_thread;
            Concurrent::Queue<uint8_t **> *_queue;
            Concurrent::Mutex *_mutex;
    };
}

#endif
