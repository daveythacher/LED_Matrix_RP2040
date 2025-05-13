/* 
 * File:   Thread.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef CONCURRENT_THREAD_H
#define CONCURRENT_THREAD_H

#include <stdint.h>

// TODO: Factory
// TODO: FreeRTOS implementation?

namespace Concurrent {
    class Thread {
        public:
            Thread(void (*func)(void *), uint32_t stack_len, uint8_t priority, void *arg = nullptr);
            ~Thread();

            static void Yield();
            static void sleep(uint32_t us);

        private:
            Thread();

            void *handle;
    };
}

#endif
