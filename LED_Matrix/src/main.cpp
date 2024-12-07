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
#include "FreeRTOS.h"
#include "task.h"

static StaticTask_t xIO_ThreadBuffer;
static StaticTask_t xWorker_ThreadBuffer;
static StaticTask_t xMultiplex_ThreadBuffer;
static StackType_t xIO_THREAD_STACK[configMINIMAL_STACK_SIZE];
static StackType_t xWORKER_THREAD_STACK[configMINIMAL_STACK_SIZE];
static StackType_t xMULTIPLEX_THREAD_STACK[configMINIMAL_STACK_SIZE];

static void __not_in_flash_func(io_thread)(void *) {
    while (1) {
        // Uses blocking pushes to FIFO.
        Serial::Node::Control::task();
        Serial::Node::Data::task();
        Serial::Protocol::task();

        watchdog_update();          // We are only interested in protecting core 0
    }
}

static void __not_in_flash_func(worker_thread)(void *) {
    while (1) {
        Matrix::Worker::work();     // Note is capable of stalling the watchdog via FIFO consumption rate. (Reduce OPs if need be.)
    }
}

static void __not_in_flash_func(multiplex_thread)(void *) {
    while (1) {
        Matrix::dma_isr();
    }
}

int main() {
    watchdog_enable(1, false);      // Make sure picotool can never be disconnected

    Serial::Node::Control::start();
    Serial::Node::Data::start();
    Serial::Protocol::start();
    Matrix::start();

    xTaskCreateStatic(io_thread, "IO", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, xIO_THREAD_STACK, &xIO_ThreadBuffer);
    xTaskCreateStatic(worker_thread, "Work", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, xWORKER_THREAD_STACK, &xWorker_ThreadBuffer);
    xTaskCreateStatic(multiplex_thread, "Multiplex", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, xMULTIPLEX_THREAD_STACK, &xMultiplex_ThreadBuffer);
	xPortStartScheduler();
}

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName ) {
    /* Force an assert. */
    configASSERT( ( volatile void * ) NULL );
}
