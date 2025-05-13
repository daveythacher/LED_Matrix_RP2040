/* 
 * File:   Queue.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef CONCURRENT_QUEUE_H
#define CONCURRENT_QUEUE_H

#include <stdint.h>

// TODO: FreeRTOS implementation?

namespace Concurrent {
    template <typename T> class Queue {
        public:
            Queue(uint8_t depth);
            ~Queue();

            void push(T val);
            T pop();
            bool available();

        private:
            Queue();

            void *handle;
    };
}

#endif
