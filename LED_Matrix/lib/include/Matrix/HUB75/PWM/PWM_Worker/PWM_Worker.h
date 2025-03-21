/* 
 * File:   PWM_Worker.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef MATRIX_PWM_WORKER_H
#define MATRIX_PWM_WORKER_H

#include <stdint.h>
#include "Matrix/Buffer.h"
#include "Matrix/Packet.h"

namespace Matrix {
    template <typename T, typename R, typename W> class PWM_Worker {
        public:
            virtual ~PWM_Worker();

            static PWM_Worker *get_worker(uint8_t scan, uint16_t steps, uint8_t columns);

            virtual void convert(Packet<R> *packet) = 0;
            virtual void convert(Buffer<T> *buffer) = 0;
    };
}

#endif
