/* 
 * File:   Watchdog.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef SYSTEM_WATCHDOG_H
#define SYSTEM_WATCHDOG_H
    
#include <stdint.h>

namespace System {
    class Watchdog {
        static void crash();
        static void reboot();
        static void enable(uint32_t ms);
    };
}

#endif
