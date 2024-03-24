/* 
 * File:   serial.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#include <stdint.h>
#include "pico/multicore.h"
#include "Serial/config.h"
#include "Matrix/matrix.h"

namespace Serial {
    static void __not_in_flash_func(test_driver_worker)() {
        static packet buffer;
        
        for (uint16_t x = 0; x < Matrix::COLUMNS; x++) {
            for (uint8_t y = 0; y < (2 * Matrix::MULTIPLEX); y++) {
                if ((x % (2 * Matrix::MULTIPLEX)) == y) {
                    buffer.data[y][x].red = 0;
                    buffer.data[y][x].green = 0;
                    buffer.data[y][x].blue = 0;
                }
                else {
                    buffer.data[y][x].red = 0xFF;
                    buffer.data[y][x].green = 0xFF;
                    buffer.data[y][x].blue = 0xFF;
                }
            }
        }
        
        Matrix::Worker::process((void *) &buffer);
        Loafer::toss();
    }

    void __not_in_flash_func(task)() {
        test_driver_worker();
    }

    void start() {
        // Do nothing
    }
}
