/* 
 * File:   Thread.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "Thread/Thread.h"
#include "FreeRTOS/FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"

namespace Thread {
    Thread::Thread() {}

    Thread::Thread(void (*func)(void *), uint32_t stack_len, uint8_t priority) {
        StackType_t *stack = new StackType_t[stack_len / sizeof(StackType_t)];
        StaticTask_t *buffer = new StaticTask_t;

        xTaskCreateStatic(func, "", stack_len / sizeof(StackType_t), NULL, priority / configMAX_PRIORITIES, stack, buffer);
    }
}