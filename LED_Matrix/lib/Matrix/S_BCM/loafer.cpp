/* 
 * File:   loafer.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#include <algorithm>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "pico/multicore.h"
#include "Serial/config.h"
#include "Matrix/matrix.h"
#include "Matrix/S_BCM/memory_format.h"
#include "Matrix/helper.h"

namespace Matrix {
    extern test2 buf[];
}

namespace Matrix::Worker {
    extern volatile bool vsync;
}

namespace Matrix::Loafer {
    static uint8_t bank = 1;

    static void __not_in_flash_func(loaf_big_data)(test2 *d) {
        for (uint8_t y = 0; y < MULTIPLEX; y++) {
            for (uint16_t x = 0; x < COLUMNS; x++) {
                // TODO
            }
        }
        
        if (!Worker::vsync) {
            bank = (bank + 1) % Serial::num_framebuffers;
            Worker::vsync = true;
        }
    }

    void __not_in_flash_func(loaf)() {        
        while(1) {
            test2 *d = (test2 *) APP::multicore_fifo_pop_blocking_inline();
            loaf_big_data(d);
        }
    }

    bool __not_in_flash_func(toss)(void *arg, bool block) {
        if (multicore_fifo_wready() || block) {
            APP::multicore_fifo_push_blocking_inline((uint32_t) arg);
            return true;
        }

        return false;
    }

    void *__not_in_flash_func(get_back_buffer)() {
        // TODO:

        return nullptr;
    }
}
