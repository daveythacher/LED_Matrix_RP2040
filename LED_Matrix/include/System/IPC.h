/* 
 * File:   IPC.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef SYSTEM_IPC_H
#define SYSTEM_IPC_H
    
#include <stdint.h>
#include "pico/util/queue.h"

namespace System {
    template <typename T> class IPC {
        public:
            IPC(uint8_t depth);

            T pop();
            void push(T val);
            bool can_push();
            bool can_pop();

        private:
            queue_t handle;
    };
}

#endif
