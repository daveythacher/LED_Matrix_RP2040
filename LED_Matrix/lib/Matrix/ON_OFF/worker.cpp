/* 
 * File:   worker.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "pico/multicore.h"
#include "Serial/config.h"
#include "Matrix/matrix.h"
#include "Matrix/ON_OFF/memory_format.h"
#include "Matrix/helper.h"

static uint8_t bank = 1;
volatile bool vsync = false;

static void __not_in_flash_func(set_pixel)(uint8_t x, uint8_t y, uint16_t r0, uint16_t g0, uint16_t b0, uint16_t r1, uint16_t g1, uint16_t b1) {    
    extern test2 buf[];
    uint16_t c[6] = { r0, g0, b0, r1, g1, b1 };
    uint8_t *p = &buf[bank][y][x + 1];
    *p = 0;

    for (uint32_t i = 0; i < 6; i++) {
        if (c[i])
            *p += 1 << i;
    }
}

void __not_in_flash_func(work)() {    
    while(1) {
        packet *p = (packet *) multicore_fifo_pop_blocking_inline();
        for (int y = 0; y < MULTIPLEX; y++)
            for (int x = 0; x < COLUMNS; x++)
                set_pixel(x, y, p->data[y][x].red, p->data[y][x].green, p->data[y][x].blue, p->data[y + MULTIPLEX][x].red, p->data[y + MULTIPLEX][x].green, p->data[y + MULTIPLEX][x].blue);
        bank = (bank + 1) % 3;
        vsync = true;
    }
}

bool __not_in_flash_func(process)(void *arg, bool block) {
    if (multicore_fifo_wready() || block) {
        multicore_fifo_push_blocking_inline((uint32_t) arg);
        return true;
    }

    return false;
}

