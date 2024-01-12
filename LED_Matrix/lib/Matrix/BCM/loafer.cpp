/* 
 * File:   loafer.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#include <stdint.h>
#include "pico/multicore.h"
#include "Serial/config.h"
#include "Matrix/matrix.h"
#include "Matrix/BCM/memory_format.h"

namespace Matrix {
    extern test2 buf[];
}

namespace Matrix::Worker {
    extern volatile bool vsync;
}

namespace Matrix::Loafer {
    static uint8_t bank = 1;    // TODO: I think we may want to share this with Matrix::Worker and Matrix now! (make volatile!!!)

    void __not_in_flash_func(toss)(void *arg) {
        // TODO: 

        if (!Worker::vsync) {
            bank = (bank + 1) % Serial::num_framebuffers;
            Worker::vsync = true;
        }
    }

    void *__not_in_flash_func(get_back_buffer)() {
        // TODO:

        return nullptr;
    }

    uint32_t __not_in_flash_func(get_buffer_size)() {
        return sizeof(test2);
    }
}
