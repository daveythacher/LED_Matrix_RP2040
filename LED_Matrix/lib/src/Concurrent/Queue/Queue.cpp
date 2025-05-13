/* 
 * File:   Queue.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "Concurrent/Queue/Queue.h"
#include "Matrix/HUB75/PWM/PWM_Packet.h"
#include "FreeRTOS.h"
#include "queue.h"

namespace Concurrent {
    template <typename T> Queue<T>::Queue() {}

    template <typename T> Queue<T>::Queue(uint8_t depth) {
        handle = xQueueCreate(depth, sizeof(T))
    }

    template <typename T> Queue<T>::~Queue() {
        if (handle != NULL) {
            vQueueDelete(handle);
        }
    }

    template <typename T> void Queue<T>::push(T val) {
        while (xQueueSend(handle, val, 1) != pdTRUE);
    }

    template <typename T> T Queue<T>::pop() {
        T val;
        while (xQueueSend(handle, &val, 1) != pdTRUE); 
        return val;
    }

    template <typename T> bool Queue<T>::available() {
        T item;
        return xQueuePeek(handle, &item, 1) == pdTRUE;
    }

    template class Queue<PWM_Packet<uint8_t> *>;
}