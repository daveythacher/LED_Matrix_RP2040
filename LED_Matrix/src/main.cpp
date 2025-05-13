/* 
 * File:   main.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include <new>
#include "System/Watchdog/Watchdog.h"
#include "Serial/Protocol/serial.h"
#include "FreeRTOS.h"
#include "task.h"

static void crash() {
    System::Watchdog::crash();
}

int main() {    
    System::Watchdog::acquire_watchdog();
    std::set_new_handler(crash);
    Serial::Protocol::Protocol::create_protocol();
	xPortStartScheduler();
}

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName ) {
    crash();
}
