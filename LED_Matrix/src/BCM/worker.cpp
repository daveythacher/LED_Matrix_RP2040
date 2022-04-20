/* 
 * File:   worker.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "pico/multicore.h"
#include "BCM/config.h"

extern uint8_t bank;
static uint8_t index_table[256][6][PWM_bits];

// TODO: Add CIE1931

static void build_table_pwm(uint8_t bits) {
    memset(index_table, 0, sizeof(index_table));
    
    for (uint32_t i = 0; i < 256; i++) {
        uint32_t steps = (uint32_t) round((i / 255.0) * (1 << bits));
        for (uint32_t j = 0; j < steps; j++) {
            for (uint32_t k = 0; k < bits; k++) {
                if ((1 << k) & j) {
                    index_table[i][0][k] = 1;
                    index_table[i][1][k] = 2;
                    index_table[i][2][k] = 4;
                    index_table[i][3][k] = 8;
                    index_table[i][4][k] = 16;
                    index_table[i][5][k] = 32;
                }
            }
        }
    }
}

// Copied from pico-sdk/src/rp2_common/pico_multicore/multicore.c
//  Allows inlining to RAM func.
static inline uint32_t __not_in_flash_func(multicore_fifo_pop_blocking_inline)(void) {
    while (!multicore_fifo_rvalid())
        __wfe();
    return sio_hw->fifo_rd;
}

static void __not_in_flash_func(set_pixel)(uint8_t x, uint8_t y, uint8_t r0, uint8_t g0, uint8_t b0, uint8_t r1, uint8_t g1, uint8_t b1) {
    extern test2 buf[];
    uint32_t *c[6] = { (uint32_t *) index_table[r0][0],  (uint32_t *) index_table[g0][1], (uint32_t *) index_table[b0][2], (uint32_t *) index_table[r1][3], (uint32_t *) index_table[g1][4], (uint32_t *) index_table[b1][5] };

    // TODO: Drop 4 pixel additions in parallel (unsafe)
    for (uint32_t i = 0; i < (PWM_bits - 2); i++) {
        uint32_t *p = (uint32_t *) &buf[bank][y][i * 4][x];
        *p = *c[0] + *c[1] + *c[2] + *c[3] + *c[4] + *c[5];
        for (uint32_t j = 0; j < 6; j++)
            ++c[j];
    }
}

void __not_in_flash_func(work)() {
    build_table_pwm(PWM_bits);
    
    while(1) {
        test *p = (test *) multicore_fifo_pop_blocking_inline();
        for (int y = 0; y < MULTIPLEX; y++)
            for (int x = 0; x < COLUMNS; x++)
                set_pixel(x, y, *p[y][x][0], *p[y][x][1], *p[y][x][2], *p[y + MULTIPLEX][x][0], *p[y + MULTIPLEX][x][1], *p[y + MULTIPLEX][x][2]);
        bank = (bank + 1) % 2;          // This will cause some screen tearing, however to avoid dynamic memory overflow and lowering FPS this was allowed.
    }
}

