/* 
 * File:   main.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include <new>
#include "hardware/watchdog.h"
#include "Serial/Serial.h"
#include "Matrix/Factory.h"

static void crash() {
    watchdog_reboot(0, 0, 0);
}

int main() {    
    std::set_new_handler(crash);
    watchdog_enable(100, false);
    Serial::Protocol::create_protocol()->work();
}
