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

    void __not_in_flash_func(toss)(void *arg) {
        loaf_big_data((test2 *) arg);
    }

    void *__not_in_flash_func(get_back_buffer)() {
        // TODO:

        return nullptr;
    }
}
