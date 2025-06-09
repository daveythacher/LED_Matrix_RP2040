/* 
 * File:   Watchdog.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "System/IPC.h"
#include "Matrix/BUS8/PWM/Packet.h"

namespace System {
    template <typename T> IPC<T>::IPC(uint8_t depth) {
        queue_init(&handle, sizeof(T), depth);
    }

    template <typename T> T IPC<T>::pop() {
        T data;

        queue_remove_blocking(&handle, &data);

        return data;
    }

    template <typename T> void IPC<T>::push(T data) {
        queue_add_blocking(&handle, data);
    }

    template <typename T> bool IPC<T>::can_push() {
        return !queue_is_full(&handle);
    }

    template <typename T> bool IPC<T>::can_pop() {
        return !queue_is_empty(&handle);
    }

    template class IPC<Matrix::BUS8::PWM::Packet *>;
}