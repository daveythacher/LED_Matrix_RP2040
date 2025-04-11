/* 
 * File:   Mutex.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef MUTEX_H
#define MUTEX_H

#include <stdint.h>

// TODO: Factory
// TODO: FreeRTOS implementation?

namespace Concurrent {
    class Mutex {
        public:
            Mutex();
            ~Mutex();

            void lock();
            void unlock();

        private:
            void *handle;
    };
}

#endif
