/* 
 * File:   Queue.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef QUEUE_H
#define QUEUE_H

#include <stdint.h>

namespace Concurrent {
    template <typename T> class Queue {
        public:
            Queue(uint8_t depth);
            // Future: Destructor

            void push(T *val);
            T pop();

        private:
            Queue();

            void *handle;
    };
}

#endif
