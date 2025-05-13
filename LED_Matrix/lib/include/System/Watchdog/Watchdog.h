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
#include "Concurrent/Thread/Thread.h"

namespace System {
    // Future: Add ability to dameonize the service within.
    //  Make this implement a service interface.
    //      Start, query, etc.
    class Watchdog {
        public:
            static Watchdog *acquire_watchdog();
            static void release_watchdog(Watchdog *wd);
            static void crash();

            void kick(uint32_t timeout_us);
            uint8_t id_num();

        protected:
            Watchdog();
            Watchdog(uint8_t id);

            static void worker(void *arg);

        private:
            uint8_t id;

            struct kick_token {
                uint8_t id;
                uint64_t timestamp;
                uint32_t interval;
            };

            struct tick_record {
                uint64_t interval;
                uint64_t last;
            };

            static const uint8_t number_of_watchdogs = sizeof(uint8_t) * 6;
            static Concurrent::Mutex *lock;
            static tick_record ticks[number_of_watchdogs];
            static Concurrent::Queue<kick_token> *queue;
            static Concurrent::Thread *thread;
            static bool isRunning;
            static uint8_t counter;
    };
}

#endif
