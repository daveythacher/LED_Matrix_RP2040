/* 
 * File:   Thread.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "Concurrent/Thread/Thread.h"
#include "FreeRTOS/FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"

namespace Concurrent {
    Thread::Thread() {}

    Thread::Thread(void (*func)(void *), uint32_t stack_len, uint8_t priority) {
        handle = NULL;
        xTaskCreate(func, NULL, stack_len / sizeof(StackType_t), NULL, priority / configMAX_PRIORITIES, handle) != pdTRUE;
    }

    Thread::~Thread() {
        if (handle != NULL) {
            vTaskDelete(handle);
        }
    }

    void Thread::Yield() {
        taskYIELD();
    }
}