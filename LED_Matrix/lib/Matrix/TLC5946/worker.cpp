/* 
 * File:   worker.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "pico/platform.h"
#include "Serial/config.h"
#include "Matrix/matrix.h"
#include "Matrix/TLC5946/memory_format.h"

static uint8_t bank = 1;
volatile bool vsync = false;

static void __not_in_flash_func(set_pixel)(uint8_t x, uint8_t y, uint16_t r0, uint16_t g0, uint16_t b0) {
    extern test2 buf[];
    
    buf[bank][y][x][0] = r0;
    buf[bank][y][x][1] = g0;
    buf[bank][y][x][2] = b0;
}

// Rewritten to use super loop. Interrupt rate can be very high, so dedicating a whole CPU core to it.
//  This is different from TLC5958 in that multiplexing is handled as part of GCLK.
void __not_in_flash_func(work)() {
    extern void matrix_gclk_task();
    
    while(1) {
        matrix_gclk_task();
    }
}

// TODO: Convert this to DMA memcpy?
// TODO: Scale 8-bit value to 12/16-bit value
void __not_in_flash_func(process)(void *arg) {
    packet *p = (packet *) arg;
    for (int y = 0; y < MULTIPLEX; y++)
        for (int x = 0; x < COLUMNS; x++)
            set_pixel(x, y, p->data[y][x].red, p->data[y][x].green, p->data[y][x].blue);
    bank = (bank + 1) % 3;
    vsync = true;
}

