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
            static void crash();

            void kick(uint8_t id);

        protected:
            Watchdog();

        private:
            struct kick_token {
                uint8_t id;
                uint64_t timestamp;
            };

            struct tick_record {
                uint64_t interval;
                uint64_t last;
            };
            
            Concurrent::Mutex *lock;
            tick_record ticks[6];
            Concurrent::Queue<kick_token> *queue;

            static Watchdog *ptr;
    };
}

#endif
