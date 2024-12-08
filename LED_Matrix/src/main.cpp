/* 
 * File:   main.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include <new>
#include "pico/multicore.h"
#include "hardware/watchdog.h"
#include "Serial/Node/Control/serial.h"
#include "Serial/Node/Data/serial.h"
#include "Serial/Protocol/serial.h"
#include "FreeRTOS.h"
#include "task.h"

static void __not_in_flash_func(io_thread)(void *) {
    while (1) {
        // Uses blocking pushes to FIFO.
        Serial::Node::Control::task();
        Serial::Node::Data::task();
        Serial::Protocol::task();

        watchdog_update();
    }
}

void crash() {
    watchdog_reboot(0, 0, 0);
}

int main() {
    static StaticTask_t xIO_Thread_Buffer;
    static StackType_t xIO_THREAD_STACK[3 * 4 * 1024 / sizeof(StackType_t)];
    static UBaseType_t xIO_Thread_Priority = configMAX_PRIORITIES - 5;
    
    watchdog_enable(1, false);      // Make sure picotool can never be disconnected
    std::set_new_handler(crash);

    Serial::Node::Control::start();
    Serial::Node::Data::start();
    Serial::Protocol::start();

    xTaskCreateStatic(io_thread, "Serial", sizeof(xIO_THREAD_STACK) / sizeof(StackType_t), NULL, xIO_Thread_Priority, xIO_THREAD_STACK, &xIO_Thread_Buffer);
	xPortStartScheduler();
}

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName ) {
    /* Force an assert. */
    configASSERT( ( volatile void * ) NULL );
}
