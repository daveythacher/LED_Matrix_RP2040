/* 
 * File:   Watchdog.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "hardware/watchdog.h"
#include "System/Watchdog.h"

namespace System {
    void Watchdog::crash() {
        reboot();
    }

    void Watchdog::reboot() {
        watchdog_reboot(0, 0, 0);
    }

    void Watchdog::enable(uint32_t ms) {
        watchdog_enable(ms, false);
    }
}