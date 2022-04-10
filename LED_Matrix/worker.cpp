#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <algorithm>
#include "pico/multicore.h"
#include "config.h"

extern uint8_t bank;
static uint8_t index_table[256][6][1 << PWM_bits];

static void build_table_pwm(uint8_t lower, uint8_t upper) {
    //assert(upper >= 0);
    //assert(upper <= 5);
    assert(upper == 0);     // Note: This is not full version
    
    uint8_t *tree_lut;
    const uint8_t tree_lut4[] = { 8, 0, 4, 12, 2, 6, 10, 14, 1, 3, 5, 7, 9, 11, 13, 15 };
    switch (lower) {
        case 4:
            tree_lut = (uint8_t *) tree_lut4;
            break;
        default:
            assert(false);
    }
    
    memset(index_table, 0, sizeof(index_table));
    
    for (uint32_t i = 0; i < 256; i++) {
        uint32_t steps = (uint32_t) round((i / 255.0) * (1 << (lower + upper)));
        for (uint32_t j = 0; j < steps;) {
            for (uint32_t k = 0; k < (uint32_t) (1 << lower) && j < steps; k++) {
                index_table[i][0][tree_lut[k]] = 1; //(index_table[i][0][tree_lut[k]] << 1) + 1;
                index_table[i][1][tree_lut[k]] = index_table[i][0][tree_lut[k]] << 1;
                index_table[i][2][tree_lut[k]] = index_table[i][1][tree_lut[k]] << 1;
                index_table[i][3][tree_lut[k]] = index_table[i][2][tree_lut[k]] << 1;
                index_table[i][4][tree_lut[k]] = index_table[i][3][tree_lut[k]] << 1;
                index_table[i][5][tree_lut[k]] = index_table[i][4][tree_lut[k]] << 1;
                j++;
            }
        }
    }
}

// Copied from pico-sdk/src/rp2_common/pico_multicore/multicore.c
//  Allows inlining to RAM func.
static inline uint32_t multicore_fifo_pop_blocking_inline(void) {
    while (!multicore_fifo_rvalid())
        __wfe();
    return sio_hw->fifo_rd;
}

static void __not_in_flash_func(set_pixel)(uint8_t x, uint8_t y, uint8_t r0, uint8_t g0, uint8_t b0, uint8_t r1, uint8_t g1, uint8_t b1) {
    extern test2 buf[];
    uint32_t *c[6] = { (uint32_t *) index_table[r0][0],  (uint32_t *) index_table[g0][1], (uint32_t *) index_table[b0][2], (uint32_t *) index_table[r1][3], (uint32_t *) index_table[g1][4], (uint32_t *) index_table[b1][5] };

    for (uint32_t i = 0; i < (1 << (PWM_bits - 2)); i++) {
        uint32_t *p = (uint32_t *) &buf[bank][y][i * 4][x];
        *p = *c[0] + *c[1] + *c[2] + *c[3] + *c[4] + *c[5];
        for (uint32_t j = 0; j < 6; j++)
            ++c[j];
    }
}

void __not_in_flash_func(work)() {
    build_table_pwm(lower, upper);
    
    while(1) {
        test *p = (test *) multicore_fifo_pop_blocking_inline();
        for (int y = 0; y < MULTIPLEX; y++)
            for (int x = 0; x < COLUMNS; x++)
                set_pixel(x, y, *p[y][x][0], *p[y][x][1], *p[y][x][2], *p[y + MULTIPLEX][x][0], *p[y + MULTIPLEX][x][1], *p[y + MULTIPLEX][x][2]);
        bank = (bank + 1) % 2;          // This will cause some screen tearing, however to avoid dynamic memory overflow and lowering FPS this was allowed.
    }
}

