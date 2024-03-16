/* 
 * File:   loafer.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#include <stdint.h>
#include "pico/multicore.h"
#include "Serial/config.h"
#include "Matrix/matrix.h"
#include "Matrix/HUB75/BCM/memory_format.h"

namespace Matrix::Worker {
    extern test2 buf[];
}

namespace Matrix::Loafer {
    static volatile bool isBackFree = true;
    static volatile bool isFrontFree = true;
    static uint8_t bank = 0;

    // Warning we can drop frames here. (There is no feedback or syncing back with front.)
    // Warning if we do not have a queue deep enough things will become undefined.
    void __not_in_flash_func(toss)() {
        isFrontFree = true;
        isBackFree = true;
        bank = (bank + 1) % Serial::num_framebuffers;
    }

    void *__not_in_flash_func(get_front_buffer)() {
        void *ptr = nullptr;

        if (isFrontFree) {
            isFrontFree = false;
            ptr = (void *) Matrix::Worker::buf[bank];
        }

        return ptr;
    }

    void *__not_in_flash_func(get_back_buffer)() {
        void *ptr = nullptr;

        if (isBackFree) {
            isBackFree = false;
            uint8_t i = (bank + 1) % Serial::num_framebuffers;
            ptr = (void *) Matrix::Worker::buf[i];
        }

        return ptr;
    }

    uint32_t __not_in_flash_func(get_buffer_size)() {
        static_assert(sizeof(test2) % 2 == 0, "Loafer buffer must be multiple of two");
        return sizeof(test2);
    }
}
