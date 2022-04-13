/* 
 * File:   serial.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"
#include "pico/multicore.h"
#include "config.h"
#include "serial.h"

static volatile bool isReady;
static test buffers[2];
static uint8_t buffer = 0;
static StaticTask_t xTaskBuffer;
static StackType_t xStack[ configMINIMAL_STACK_SIZE ];

static void serial_task(void *arg) {
    if (isReady) {
        multicore_fifo_push_blocking((uint32_t) &buffers[(buffer + 1) % 2]);
        isReady = false;
    }
}

void serial_start() {
    extern void work();
    multicore_launch_core1(work);
    xTaskCreateStatic( serial_task, "SER", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, xStack, &xTaskBuffer );

    // TODO:
}   

