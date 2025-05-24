/* 
 * File:   main.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include <new>
#include "System/Watchdog/Watchdog.h"
#include "Serial/Serial.h"
#include "Matrix/Factory.h"

static void crash() {
    System::Watchdog::crash();
}

static void core1() {
    Matrix::Factory::get_matrix()->work();
}

int main() {    
    System::Watchdog::acquire_watchdog();
    std::set_new_handler(crash);
    Serial::Protocol::Protocol::create_protocol();
}
