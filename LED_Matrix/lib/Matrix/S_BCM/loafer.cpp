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
    static uint8_t bank = 1;
    volatile bool vsync = false;
}

namespace Matrix::Loafer {

    static void __not_in_flash_func(loaf_big_data)(Serial::packet *p) {
        for (uint8_t y = 0; y < MULTIPLEX; y++) {
            for (uint16_t x = 0; x < COLUMNS; x++) {
                // TODO
            }
        }
        
        if (!vsync) {
            bank = (bank + 1) % 3;
            vsync = true;
        }
    }

    void __not_in_flash_func(loaf)() {        
        while(1) {
            Serial::data *p = (Serial::data *) APP::multicore_fifo_pop_blocking_inline();
            loaf_big_data(p);
        }
    }

    bool __not_in_flash_func(toss)(void *arg, bool block) {
        if (multicore_fifo_wready() || block) {
            APP::multicore_fifo_push_blocking_inline((uint32_t) arg);
            return true;
        }

        return false;
    }
}
