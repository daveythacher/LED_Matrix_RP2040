/* 
 * File:   main.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include <new>
#include "hardware/watchdog.h"
#include "Serial/Protocol/serial.h"
#include "FreeRTOS.h"
#include "task.h"

void crash() {
    watchdog_reboot(0, 0, 0);
}

int main() {    
    watchdog_enable(1, false);
    std::set_new_handler(crash);
    Serial::Protocol::Protocol::create_protocol();
	xPortStartScheduler();
}

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName ) {
    crash();
}
