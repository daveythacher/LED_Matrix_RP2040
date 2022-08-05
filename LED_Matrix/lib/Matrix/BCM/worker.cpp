/* 
 * File:   worker.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "pico/multicore.h"
#include "Matrix/BCM/config.h"

static uint8_t bank = 1;
volatile bool vsync = false;
static uint8_t index_table[256][6][PWM_bits];

static void build_table_pwm(uint8_t bits) {
    memset(index_table, 0, sizeof(index_table));
    
    for (uint32_t i = 0; i < 256; i++) {
        uint32_t r = round(pow((i / 255.0), 1.0 / RED_GAMMA) * ((1 << bits) - 1));
        uint32_t g = round(pow((i / 255.0), 1.0 / GREEN_GAMMA) * ((1 << bits) - 1));
        uint32_t b = round(pow((i / 255.0), 1.0 / BLUE_GAMMA) * ((1 << bits) - 1));

        for (uint32_t j = 0; j < bits; j++) {
            if ((1 << j) & r) {
                index_table[i][0][j] = 1;
                index_table[i][3][j] = 8;
            }
            if ((1 << j) & g) {
                index_table[i][1][j] = 2;
                index_table[i][4][j] = 16;
            }
            if ((1 << j) & b) {
                index_table[i][2][j] = 4;
                index_table[i][5][j] = 32;
            }
        }
    }
}

// Copied from pico-sdk/src/rp2_common/pico_multicore/multicore.c
//  Allows inlining to RAM func. (Currently linker is copy-to-RAM)
//  May be better to use -ffunction-sections and -fdata-sections with custom linker script
//      .ram_text : {
//          *(.text.multicore_fifo_pop_blocking)
//          . = ALIGN(4);
//      } > RAM AT> FLASH
//      .text : {
static inline uint32_t __not_in_flash_func(multicore_fifo_pop_blocking_inline)(void) {
    while (!multicore_fifo_rvalid())
        __wfe();
    return sio_hw->fifo_rd;
}

static void __not_in_flash_func(set_pixel)(uint8_t x, uint8_t y, uint8_t r0, uint8_t g0, uint8_t b0, uint8_t r1, uint8_t g1, uint8_t b1) {
    extern test2 buf[];
    uint8_t *c[6] = { index_table[r0][0],  index_table[g0][1], index_table[b0][2], index_table[r1][3], index_table[g1][4], index_table[b1][5] };

    for (uint32_t i = 0; i < PWM_bits; i++) {
        uint8_t *p = &buf[bank][y][i][x + 1];
        *p = *c[0] + *c[1] + *c[2] + *c[3] + *c[4] + *c[5];
        for (uint32_t j = 0; j < 6; j++)
            ++c[j];
    }
}

void __not_in_flash_func(work)() {
    extern void isr_start_core1();
    build_table_pwm(PWM_bits);
    isr_start_core1();
    
    while(1) {
        test *p = (test *) multicore_fifo_pop_blocking_inline();
        for (int y = 0; y < MULTIPLEX; y++)
            for (int x = 0; x < COLUMNS; x++)
                set_pixel(x, y, (*p)[y][x][0], (*p)[y][x][1], (*p)[y][x][2], (*p)[y + MULTIPLEX][x][0], (*p)[y + MULTIPLEX][x][1], (*p)[y + MULTIPLEX][x][2]);
        bank = (bank + 1) % 3;
        vsync = true;
    }
}

