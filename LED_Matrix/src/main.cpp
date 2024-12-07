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

// Future: Tune
static const uint16_t default_stack_size = 4 * 1024;

static StaticTask_t xIO_Thread_Buffer;
static StaticTask_t xWorker_Thread_Buffer;
static StaticTask_t xMultiplex_Thread_Buffer;
static StackType_t xIO_THREAD_STACK[default_stack_size];
static StackType_t xWORKER_THREAD_STACK[default_stack_size];
static StackType_t xMULTIPLEX_THREAD_STACK[default_stack_size];
static UBaseType_t xIO_Thread_Priority = tskIDLE_PRIORITY + 5;
static UBaseType_t xWorker_Thread_Priority = tskIDLE_PRIORITY + 1;
static UBaseType_t xMultiplex_Thread_Priority = tskIDLE_PRIORITY + 10;


static void __not_in_flash_func(io_thread)(void *) {
    while (1) {
        // Uses blocking pushes to FIFO.
        Serial::Node::Control::task();
        Serial::Node::Data::task();
        Serial::Protocol::task();

        watchdog_update();
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

    xTaskCreateStatic(io_thread, "Serial", sizeof(xIO_THREAD_STACK) / sizeof(StackType_t), NULL, xIO_Thread_Priority, xIO_THREAD_STACK, &xIO_Thread_Buffer);
    xTaskCreateStatic(worker_thread, "Matrix_Worker", sizeof(xWORKER_THREAD_STACK) / sizeof(StackType_t), NULL, xWorker_Thread_Priority, xWORKER_THREAD_STACK, &xWorker_Thread_Buffer);
    xTaskCreateStatic(multiplex_thread, "Matrix_Multiplex", sizeof(xMULTIPLEX_THREAD_STACK) / sizeof(StackType_t), NULL, xMultiplex_Thread_Priority, xMULTIPLEX_THREAD_STACK, &xMultiplex_Thread_Buffer);
	xPortStartScheduler();
}

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName ) {
    /* Force an assert. */
    configASSERT( ( volatile void * ) NULL );
}
