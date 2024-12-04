/* 
 * File:   main.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "pico/multicore.h"
#include "hardware/watchdog.h"
#include "Matrix/matrix.h"
#include "Serial/Node/Control/serial.h"
#include "Serial/Node/Data/serial.h"
#include "Serial/Protocol/serial.h"
#include "ISR/isr.h"
#include "FreeRTOS.h"
#include "task.h"

static StaticTask_t xLOOP_CORE0Buffer;
static StaticTask_t xLOOP_CORE1Buffer;
static StackType_t xLOOP_CORE0_STACK[configMINIMAL_STACK_SIZE];
static StackType_t xLOOP_CORE1_STACK[configMINIMAL_STACK_SIZE];

static void __not_in_flash_func(loop_core0)(void *args) {
    while (1) {
        // Uses blocking pushes to FIFO.
        Serial::Node::Control::task();
        Serial::Node::Data::task();
        Serial::Protocol::task();

        watchdog_update();          // We are only interested in protecting core 0
    }
}

static void __not_in_flash_func(loop_core1)(void *args) {
    Matrix::start();                // Note ISR logic should be detected by watchdog. (FIFO will fill up and Matrix::Worker::work will stall.)
    APP::isr_start_core1();         // Matrix ISRs are allocated to this core. (These will slow the FIFO consumption rate.)

    while (1) {
        Matrix::Worker::work();     // Note is capable of stalling the watchdog via FIFO consumption rate. (Reduce OPs if need be.)
    }
}

int main() {
    watchdog_enable(1, false);      // Make sure picotool can never be disconnected

    Serial::Node::Control::start();
    Serial::Node::Data::start();
    Serial::Protocol::start();
    //multicore_launch_core1(loop_core1);
    //loop_core0();

    xTaskCreateStatic(loop_core0, "core0", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, xLOOP_CORE0_STACK, &xLOOP_CORE0Buffer);
    xTaskCreateStatic(loop_core1, "core1", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, xLOOP_CORE1_STACK, &xLOOP_CORE1Buffer);
	xPortStartScheduler();
}

// TODO: Sort this out
void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName ) {
    ( void ) pcTaskName;
    ( void ) pxTask;

    /* Run time stack overflow checking is performed if
    configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
    function is called if a stack overflow is detected. */

    /* Force an assert. */
    configASSERT( ( volatile void * ) NULL );
}
