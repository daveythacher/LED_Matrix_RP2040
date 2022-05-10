/* 
 * File:   worker.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <algorithm>
#include <queue>
#include "pico/multicore.h"
#include "config.h"

/*
    This file implements Scrambled PWM (S-PWM) in software using memory look up tables.
    This is done to improve performance of software creating bitplanes. (set_pixel)
    
    This version of S-PWM does not make use of upper bits. This is done to simplify the matrix state
    machine and memory accesses. There is a version of this which makes use of the upper bits, however
    that is not provided here. There is also a version of S-PWM which works off BCM, which requires 
    special consideration.
    
    Normally this is done by a bunch of compares which are incredibly slow. To speed this up
    index_table is used which contains a bit mapping for every RGB color. A bit mapping also exists
    for every bit in the HUB75 connector, which was chosen to make performance uniform.
    Now set_pixel will just sum the bits together and store the results. Since this is a 32-bit processor
    4 bits can be combine and copied in parallel without SIMD.
    
    This should allow decent runtime performance even on Cortex-M0+. This code base realies heavily on
    RAM and does not leave much room in reserve on most microcontrollers. This code also requires decent 
    instruction memory performance. All generation logic is non-critical and is performed slowly at boot.
    
    This was discovered when messing around with SIMD. It was found SIMD was very effective in removing
    compares from traditional PWM without BCM. The compares could be accelerated with SIMD, but the merge
    operation was hard to accelerate. At this point it was realized that using memory could achieve the
    same rough result. Till this point FPGA was assumed to be the only real way to improve this.
    
    This is very memory/IO intensive, and this should not be ignored. There are intermediate versions which
    use less memory, however these rely on compare operations. Again this compare operations are expensive.
    This represents a memory based solution instead of a computation solution.
    
    SIMD/NEON may allow for better computation acceleration. This solutions is fairly close to this, however
    it is possible to get better acceleration with SIMD. Currently the acceleration limited to 32-bit 
    operations. SIMD relies heavily on memory performance and some implementations may show weaker results.
    
    This is a strange outcome as the CPU is not expected to be very good at these type of operations.
    
    Note this code base does not support full S-PWM however some sections are more capable than others.
*/

extern uint8_t bank;
static uint8_t index_table[256][6][1 << PWM_bits];
static void build_tree_lut(uint8_t **tree_lut, uint8_t lower);
static void destroy_tree_lut(uint8_t *tree_lut);

static void build_table_pwm(uint8_t lower, uint8_t upper) {    
    uint8_t *tree_lut = nullptr;
    build_tree_lut(&tree_lut, lower);
    memset(index_table, 0, sizeof(index_table));
    
    for (uint32_t i = 0; i < 256; i++) {
        uint32_t steps;
        if (!USE_CIE1931)
            steps = (uint32_t) round((i / 255.0) * (1 << (lower + upper)));
        else {
            float x = i * 100 / 255.0;
            if (x <= 8)
                steps = round((x / 902.3) * (1 << (lower + upper)));
            else
                steps = round(pow((x + 16) / 116.0, 3) * (1 << (lower + upper)));
        }
        for (uint32_t j = 0; j < steps; j++) {
            index_table[i][0][tree_lut[j]] = (index_table[i][0][tree_lut[j]] << 1) + 1;
            index_table[i][1][tree_lut[j]] = index_table[i][0][tree_lut[j]] << 1;
            index_table[i][2][tree_lut[j]] = index_table[i][1][tree_lut[j]] << 1;
            index_table[i][3][tree_lut[j]] = index_table[i][2][tree_lut[j]] << 1;
            index_table[i][4][tree_lut[j]] = index_table[i][3][tree_lut[j]] << 1;
            index_table[i][5][tree_lut[j]] = index_table[i][4][tree_lut[j]] << 1;
        }
    }
    
    destroy_tree_lut(tree_lut);
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
    uint8_t *c[6] = { index_table[r0][0], index_table[g0][1], index_table[b0][2], index_table[r1][3], index_table[g1][4], index_table[b1][5] };
    uint32_t num = x / (COLUMNS / 8);
    uint32_t n = (x % (COLUMNS / 8)) * ((num % 2) + 1);

    for (uint32_t i = 0; i < (1 << lower); i++) {
        uint8_t *p = &buf[bank][y][i][num][n];
        *p = *c[0] + *c[1] + *c[2] + *c[3] + *c[4] + *c[5];
        for (uint32_t j = 0; j < 6; j++)
            ++c[j];
    }
}

void __not_in_flash_func(work)() {
    //static_assert(upper >= 0 && upper <= 5);    // 0 to log2(uint32_t)
    static_assert(upper == 0);                  // This is not the full version, this is refresh hack for complexity reduction
    static_assert(lower <= 8);                  // tree_lut uses uint8_t
    static_assert(lower >= 1);                  // set_pixel needs atleast one bit, this is speed hack for custom version
    
    build_table_pwm(lower, upper);
    
    while(1) {
        test *p = (test *) multicore_fifo_pop_blocking_inline();
        for (int y = 0; y < MULTIPLEX; y++)
            for (int x = 0; x < COLUMNS; x++)
                set_pixel(x, y, *p[y][x][0], *p[y][x][1], *p[y][x][2], *p[y + MULTIPLEX][x][0], *p[y + MULTIPLEX][x][1], *p[y + MULTIPLEX][x][2]);
        bank = (bank + 1) % 2;          // This will cause some screen tearing, however to avoid dynamic memory overflow and lowering FPS this was allowed.
    }
}

void build_tree_lut(uint8_t **tree_lut, uint8_t lower) {
#ifdef USE_MALLOC_TREE_LUT
    *tree_lut = (uint8_t *) malloc(1 << lower);
    if (*tree_lut == NULL)
        assert(false);
    
    if (lower == 0)
        *tree_lut[0] = 0;
    else {
        std::queue<uint8_t> queue;
        std::queue<uint8_t> tmp;
        uint8_t index = 2;
        for (uint8_t i = lower; i > 0; i--) {
            if (i == lower) {
                *tree_lut[0] = 1 << (i - 1);
                *tree_lut[1] = 0;
                queue.push(*tree_lut[0]);
            }
            else {
                tmp = queue;
                queue = {};
                while (!tmp.empty()) {
                    uint8_t var = tmp.front();
                    *tree_lut[index] = var - (1 << (i - 1));
                    *tree_lut[index + 1] = var + (1 << (i - 1));
                    queue.push(*tree_lut[index]);
                    queue.push(*tree_lut[index + 1]);
                    index += 2;
                    tmp.pop();
                }
            }
        }
    }
#else
    static const uint8_t tree_lut0[] = { 0 };
    static const uint8_t tree_lut1[] = { 1, 0 };
    static const uint8_t tree_lut2[] = { 2, 0, 1, 3 };
    static const uint8_t tree_lut3[] = { 4, 0, 2, 6, 1, 3, 5, 7 };
    static const uint8_t tree_lut4[] = { 8, 0, 4, 12, 2, 6, 10, 14, 1, 3, 5, 7, 9, 11, 13, 15 };
    static const uint8_t tree_lut5[] = { 16, 0, 8, 24, 4, 12, 20, 28, 2, 6, 10, 14, 18, 22, 26, 30, 1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31 };
    static const uint8_t tree_lut6[] = { 32, 0, 16, 48, 8, 24, 40, 56, 4, 12, 20, 28, 36, 44, 52, 60, 2, 6, 10, 14, 18, 22, 26, 30, 34, 38, 42, 46, 50, 54, 58, 62, 1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31, 33, 35, 37, 39, 41, 43, 45, 47, 49, 51, 53, 55, 57, 59, 61, 63 };
    static const uint8_t tree_lut7[] = { 64, 0, 32, 96, 16, 48, 80, 112, 8, 24, 40, 56, 72, 88, 104, 120, 4, 12, 20, 28, 36, 44, 52, 60, 68, 76, 84, 92, 100, 108, 116, 124, 2, 6, 10, 14, 18, 22, 26, 30, 34, 38, 42, 46, 50, 54, 58, 62, 66, 70, 74, 78, 82, 86, 90, 94, 98, 102, 106, 110, 114, 118, 122, 126, 1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31, 33, 35, 37, 39, 41, 43, 45, 47, 49, 51, 53, 55, 57, 59, 61, 63, 65, 67, 69, 71, 73, 75, 77, 79, 81, 83, 85, 87, 89, 91, 93, 95, 97, 99, 101, 103, 105, 107, 109, 111, 113, 115, 117, 119, 121, 123, 125, 127 };

    switch (lower) {
        case 0:
            *tree_lut = (uint8_t *) tree_lut0;
            break;
        case 1:
            *tree_lut = (uint8_t *) tree_lut1;
            break;
        case 2:
            *tree_lut = (uint8_t *) tree_lut2;
            break;
        case 3:
            *tree_lut = (uint8_t *) tree_lut3;
            break;
        case 4:
            *tree_lut = (uint8_t *) tree_lut4;
            break;
        case 5:
            *tree_lut = (uint8_t *) tree_lut5;
            break;
        case 6:
            *tree_lut = (uint8_t *) tree_lut6;
            break;
        case 7:
            *tree_lut = (uint8_t *) tree_lut7;
            break;
        default:
            assert(false);
    }
#endif
}

void destroy_tree_lut(uint8_t *tree_lut) {
#ifdef USE_MALLOC_TREE_LUT
    free(tree_lut);
#endif
}

