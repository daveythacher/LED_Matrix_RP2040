/* 
 * File:   worker.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "pico/multicore.h"
#include "MBI5153/config.h"

extern uint8_t bank;
extern test2 buf[];
static uint16_t index_table[256];

static void build_table_pwm(uint8_t bits) {
    memset(index_table, 0, sizeof(index_table));
    
    for (uint32_t i = 0; i < 256; i++) {
        if (!USE_CIE1931)
            index_table[i] = (uint32_t) round((i / 255.0) * ((1 << bits) - 1));
        else {
            float x = i * 100 / 255.0;
            if (x <= 8)
                index_table[i] = round((x / 902.3) * ((1 << bits) - 1));
            else
                index_table[i] = round(pow((x + 16) / 116.0, 3) * ((1 << bits) - 1));
        }
    }
}

void __not_in_flash_func(set_pixel)(uint8_t x, uint8_t y, uint8_t r0, uint8_t g0, uint8_t b0, uint8_t r1, uint8_t g1, uint8_t b1) {
    buf[bank][0][y][x % 16][x / 16] = index_table[r0];
    buf[bank][1][y][x % 16][x / 16] = index_table[g0];
    buf[bank][2][y][x % 16][x / 16] = index_table[b0];
    buf[bank][3][y][x % 16][x / 16] = index_table[r1];
    buf[bank][4][y][x % 16][x / 16] = index_table[g1];
    buf[bank][5][y][x % 16][x / 16] = index_table[b1];
}

// Copied from pico-sdk/src/rp2_common/pico_multicore/multicore.c
//  Allows inlining to RAM func.
static inline uint32_t __not_in_flash_func(multicore_fifo_pop_blocking_inline)(void) {
    while (!multicore_fifo_rvalid())
        __wfe();
    return sio_hw->fifo_rd;
}

void __not_in_flash_func(work)() {
    extern void isr_start();
    build_table_pwm(PWM_bits);
    isr_start();
    
    while(1) {
        test *p = (test *) multicore_fifo_pop_blocking_inline();
        for (int y = 0; y < MULTIPLEX; y++)
            for (int x = 0; x < COLUMNS; x++)
                set_pixel(x, y, (*p)[y][x][0], (*p)[y][x][1], (*p)[y][x][2], (*p)[y + MULTIPLEX][x][0], (*p)[y + MULTIPLEX][x][1], (*p)[y + MULTIPLEX][x][2]);
        bank = (bank + 1) % 2;          // This will cause some screen tearing, however to avoid dynamic memory overflow and lowering FPS this was allowed.
    }
}

