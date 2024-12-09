/* 
 * File:   Thread.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef THREAD_H
#define THREAD_H

#include <stdint.h>

namespace Concurrent {
    class Thread {
        public:
            Thread(void (*func)(void *), uint32_t stack_len, uint8_t priority);
            ~Thread();

        private:
            Thread();

            void *handle;
            void *stack;
            void *buffer;
    };
}

#endif
