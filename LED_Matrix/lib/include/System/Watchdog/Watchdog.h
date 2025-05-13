/* 
 * File:   Watchdog.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef SYSTEM_WATCHDOG_H
#define SYSTEM_WATCHDOG_H

#include <stdint.h>
#include "Concurrent/Mutex/Mutex.h"
#include "Concurrent/Queue/Queue.h"

namespace System {
    class Watchdog {
        public:
            static Watchdog *acquire_watchdog();

            void kick(uint8_t id);

        protected:
            Watchdog();

        private:
            Watchdog *ptr;
            Concurrent::Mutex *lock;
            Concurrent::Queue<uint64_t> *queue;
    };
}

#endif
