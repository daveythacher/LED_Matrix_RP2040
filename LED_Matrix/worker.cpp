#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <algorithm>
#include "pico/multicore.h"
#include "config.h"

static uint8_t *index_table[256][6];

static void build_table_pwm(uint8_t lower, uint8_t upper) {
    assert(upper == 0);     // This is not full version
    
    for (uint32_t i = 0; i < (uint32_t) (1 << (lower + upper)); i++) {
        uint32_t counter = 0;
        uint16_t temp[1 << std::min(lower + upper - 4, 1)];
        memset(temp, 0, 1 << std::max(lower + upper - 4, 1));
        if (upper == 0) {
            for (int32_t l = lower - 1; l >= 0; l--) {
                for (uint32_t j = 1 << l; j < (uint32_t) (1 << (lower + upper)); j += 1 << l) {
                    if ((j / (1 << l)) % 2) {
                        if (counter < i)
                            temp[j / 16] |= 1 << (j % 16);
                        counter++;
                    }
                }
            }
        }
        else {
            for (int32_t l = upper; l >= 0; l--) {
                for (uint32_t k = 0; k < (uint32_t) (1 << lower); k++) {
                    for (uint32_t j = k; j < (uint32_t) (1 << (lower + upper)); j += 1 << (l + upper)) {
                        if (counter < i)
                            temp[j / 16] |= 1 << (j % 16);
                        counter++;
                    }
                }
            }
        }
        for (int32_t j = 1 << std::max(lower + upper - 4, 1); j > 0; j--) {
            //*table = temp[j - 1];
            //++table;
            // TODO: 
        }
    }
}


static void build_tables() {
    for (uint32_t i = 0; i < 256; i++)
        for (uint32_t j = 0; j < 6; j++)
            index_table[i][j] = (uint8_t *) malloc(1 << PWM_bits);
    build_table_pwm(lower, upper);
}

// Copied from pico-sdk/src/rp2_common/pico_multicore/multicore.c
//  Allows inlining to RAM func.
static inline uint32_t multicore_fifo_pop_blocking_inline(void) {
    while (!multicore_fifo_rvalid())
        __wfe();
    return sio_hw->fifo_rd;
}

void set_pixel(uint8_t x, uint8_t y, uint8_t r0, uint8_t g0, uint8_t b0, uint8_t r1, uint8_t g1, uint8_t b1) {
    extern uint8_t bank;
    extern test2 buf[];
    uint32_t *c[6] = { (uint32_t *) index_table[r0][0],  (uint32_t *) index_table[g0][1], (uint32_t *) index_table[b0][2], (uint32_t *) index_table[r1][3], (uint32_t *) index_table[g1][4], (uint32_t *) index_table[b1][5] };

    for (uint32_t i = 0; i < (1 << (PWM_bits - 2)); i++) {
        uint32_t *p = (uint32_t *) &buf[bank][y][i * 4][x];
        *p = *c[0] + *c[1] + *c[2] + *c[3] + *c[4] + *c[5];
        for (uint32_t j = 0; j < 6; j++)
            ++c[j];
    }
}

void work() {
    extern void matrix_switch();
    build_tables();
    
    while(1) {
        test *p = (test *) multicore_fifo_pop_blocking_inline();
        for (int y = 0; y < MULTIPLEX; y++)
            for (int x = 0; x < COLUMNS; x++)
                set_pixel(x, y, *p[y][x][0], *p[y][x][1], *p[y][x][2], *p[y + MULTIPLEX][x][0], *p[y + MULTIPLEX][x][1], *p[y + MULTIPLEX][x][2]);
        free(p);
        matrix_switch();
    }
}
