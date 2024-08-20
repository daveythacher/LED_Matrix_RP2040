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
    void __not_in_flash_func(task)() {
        static volatile packet buffers[num_packets];
        static volatile uint8_t buffer = 0;
        
        for (uint16_t x = 0; x < Matrix::COLUMNS; x++) {
            for (uint8_t y = 0; y < (2 * Matrix::MULTIPLEX); y++) {
                if ((x % (2 * Matrix::MULTIPLEX)) == y) {
                    buffers[buffer].data[y][x].red = 0;
                    buffers[buffer].data[y][x].green = 0;
                    buffers[buffer].data[y][x].blue = 0;
                }
                else {
                    buffers[buffer].data[y][x].red = 0xFF;
                    buffers[buffer].data[y][x].green = 0xFF;
                    buffers[buffer].data[y][x].blue = 0xFF;
                }
            }
        }
        
        buffer = (buffer + 1) % num_packets;   
        Matrix::Worker::process(&buffers[(buffer + 1) % num_packets]);
    }

    void start() {
        // Do nothing
    }
}
