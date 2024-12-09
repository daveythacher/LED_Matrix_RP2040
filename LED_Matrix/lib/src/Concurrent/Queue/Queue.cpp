/* 
 * File:   Queue.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "Concurrent/Queue/Queue.h"
#include "Serial/config.h"
#include "FreeRTOS/FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "queue.h"

namespace Concurrent {
    template <typename T> Queue<T>::Queue() {}

    template <typename T> Queue<T>::Queue(uint8_t depth) {
        uint8_t *stack = new uint8_t[depth * sizeof(T)];
        StaticQueue_t *queue = new StaticQueue_t;
        handle = xQueueCreateStatic(depth, sizeof(T), area, queue)
    }

    template <typename T> void Queue<T>::push(T *val) {
        while (xQueueSend(handle, val, 1) != pdTRUE);
    }

    template <typename T> T Queue<T>::pop() {
        T val;
        while (xQueueSend(handle, &val, 1) != pdTRUE); 
        return val;
    }

    template class Queue<Serial::packet>;
}