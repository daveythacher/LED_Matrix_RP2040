/* 
 * File:   PWM_Worker.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef MATRIX_PWM_WORKER_H
#define MATRIX_PWM_WORKER_H

#include <stdint.h>
#include "Matrix/Packet.h"

namespace Matrix {
    class PWM_Worker {
        public:
            virtual ~PWM_Worker();

            static PWM_Worker *get_worker();

            virtual void convert(Packet *packet) = 0;
    };
}

#endif
