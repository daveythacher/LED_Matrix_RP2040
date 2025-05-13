#include "hardware/watchdog.h"
#include "hardware/timer.h"
#include "System/Watchdog/Watchdog.h"

namespace System {
    Concurrent::Mutex *Watchdog::lock;
    Watchdog::tick_record Watchdog::ticks[Watchdog::number_of_watchdogs];
    Concurrent::Queue<Watchdog::kick_token> *Watchdog::queue;
    bool Watchdog::isRunning = false;
    uint8_t Watchdog::counter = 0;

    Watchdog::Watchdog() {
        // Do nothing
    }
    
    Watchdog::Watchdog(uint8_t num) {
        id = num;
    }

    Watchdog *Watchdog::acquire_watchdog() {
        if (!isRunning) {
            thread = new Concurrent::Thread(worker, 4096, 255, nullptr); // TODO: Update
            // TODO: Finish
            //  Queue, etc.
        }

        for (uint8_t i = 0; i < number_of_watchdogs; i++) {
            if ((counter & (1 << i)) == 0) {
                counter |= 1 << i;
                return new Watchdog(i);
            }
        }

        return nullptr;
    }

    void Watchdog::release_watchdog(Watchdog *wd) {
        if (wd != nullptr) {
            uint8_t i = wd->id_num();

            if (i < number_of_watchdogs) {
                counter &= 0xFF ^ (1 << i);
            }

            delete wd;
        }
    }

    uint8_t Watchdog::id_num() {
        return id;
    }

    void Watchdog::crash() {
        watchdog_reboot(0, 0, 0);
    }

    void Watchdog::kick(uint32_t interval) {
        kick_token token;

        token.id = id;
        token.timestamp = time_us_64();
        token.interval = interval;

        // Future: Add Frame here
        lock->lock();
        queue->push(token);
        lock->unlock();
    }

    void Watchdog::worker(void *) {
        watchdog_enable(1, false);

        while (1) {
            // Check FIFO for messages (aka kicks)
            if (queue->available()) {
                // Future: Add Frame here
                lock->lock();
                kick_token token = queue->pop();
                lock->unlock();
                ticks[token.id].last = token.timestamp;
            }

            // Check to see if the watchdog should go off
            uint64_t timestamp = time_us_64();
            for (uint8_t i = 0; i < number_of_watchdogs; i++) {
                if (((ticks[i].last + ticks[i].interval) > timestamp) && (counter & (1 << i))) {
                    Watchdog::crash();
                }
            }

            watchdog_update();
        }
    }
}