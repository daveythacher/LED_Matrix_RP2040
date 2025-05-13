/* 
 * File:   Mutex.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef CONCURRENT_MUTEX_H
#define CONCURRENT_MUTEX_H

#include <stdint.h>

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
