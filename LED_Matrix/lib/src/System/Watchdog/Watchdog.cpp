#include "hardware/watchdog.h"
#include "hardware/timer.h"
#include "System/Watchdog/Watchdog.h"

namespace System {
    Watchdog *Watchdog::ptr = nullptr;

    Watchdog::Watchdog() {
        // TODO:
        thread = new Concurrent::Thread(worker, 4096, 255, this); // TODO: Update
    }

    Watchdog *Watchdog::acquire_watchdog() {
        if (ptr == nullptr) {
            ptr = new Watchdog();
        }

        return ptr;
    }

    void Watchdog::crash() {
        watchdog_reboot(0, 0, 0);
    }

    void Watchdog::kick(uint8_t id) {
        kick_token token;

        if (id > 6) {
            return;
        }

        token.id = id;
        token.timestamp = time_us_64();
        queue->push(token);
    }

    void Watchdog::worker(void *arg) {
        Watchdog *ptr = static_cast<Watchdog *>(arg);
        watchdog_enable(1, false);

        while (1) {
            // Check FIFO for messages (aka kicks)
            if (ptr->queue->available()) {
                kick_token token = ptr->queue->pop();
                ptr->ticks[token.id].last = token.timestamp;
            }

            // Check to see if the watchdog should go off
            uint64_t timestamp = time_us_64();
            for (uint8_t i = 0; i < sizeof(ticks) / sizeof(tick_record); i++) {
                if (ptr->ticks[i].last + ptr->ticks[i].interval > timestamp) {
                    Watchdog::crash();
                }
            }

            watchdog_update();
        }
    }
}