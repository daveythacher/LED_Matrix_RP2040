/* 
 * File:   loafer.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#include <stdint.h>
#include "pico/multicore.h"
#include "Serial/config.h"
#include "Matrix/matrix.h"
#include "Matrix/S_PWM/memory_format.h"

namespace Matrix {
    extern test2 buf[];
}

namespace Matrix::Worker {
    extern volatile bool vsync;
}

namespace Matrix::Loafer {
    static volatile uint8_t count = 0;

    static void increment_count() {
        ++count;
        while (count >= Serial::num_framebuffers);
    }

    static void decrement_count() {
        if (count == 0)
            while (1);
        else
            --count;
    }

    bool __not_in_flash_func(toss)(void *arg, bool block) {
        if (block) {
            while(Worker::vsync);

            Worker::vsync = true;
            decrement_count();
            return false;
        }
        else {
            if (!Worker::vsync) {
                Worker::vsync = true;
                decrement_count();
                return false;
            }
            else {
                decrement_count();
                return true;
            }
        }
    }

    void *__not_in_flash_func(get_back_buffer)() {
        static uint8_t bank = 1;  
        
        // TODO:
        
        bank = (bank + 1) % Serial::num_framebuffers;
        increment_count();
        return nullptr;
    }
}
