#include "hardware/watchdog.h"
#include "hardware/timer.h"
#include "System/Watchdog/Watchdog.h"

namespace System {
    Watchdog *Watchdog::ptr = nullptr;

    Watchdog::Watchdog() {
        // TODO:
        watchdog_enable(1, false);
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
}