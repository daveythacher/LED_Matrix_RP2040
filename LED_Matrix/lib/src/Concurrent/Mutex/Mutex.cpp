/* 
 * File:   Mutex.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "Concurrent/Mutex/Mutex.h"
#include "FreeRTOS.h"
#include "semphr.h"

namespace Concurrent {
    Mutex::Mutex() {
        handle = xSemaphoreCreateMutex();
    }

    Mutex::~Mutex() {
        if (handle != NULL) {
            unlock();
            vSemaphoreDelete(handle);
        }
    }

    void Mutex::lock() {
        while (xSemaphoreTake(handle, 1) != pdTRUE);
    }

    void Mutex::unlock() {
        xSemaphoreGive(handle);
    }
}