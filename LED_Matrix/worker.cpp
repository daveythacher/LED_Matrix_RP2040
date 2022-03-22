#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <algorithm>
#include "config.h"

static const int lower = std::min((int) round(log2(MAX_REFRESH / FPS)), PWM_bits);
static const int upper = PWM_bits - lower;
static const int offset = std::max(PWM_bits - 4, 0);
uint16_t table[1 << (PWM_bits + offset)];
uint16_t index_table[1 << 8];
static test2 buf[2];

static void build_table_pwm(uint16_t *table, uint8_t lower, uint8_t upper) {
    assert(upper != 0);     // This is not full version
    
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
        for (uint16_t j = 1 << std::max(lower + upper - 4, 1); j > 0; j--) {
            *table = temp[j - 1];
            ++table;
        }
    }
}


static void build_tables() {
    for (uint16_t i = 0; i < 256; i++)
        index_table[i] = (int) round((i / 255.0) * ((1 << PWM_bits) - 1)); 
    build_table_pwm(table, lower, upper);
}

static void set_pixel(uint8_t x, uint8_t y, uint8_t r0, uint8_t g0, uint8_t b0, uint8_t r1, uint8_t g1, uint8_t b1) {
    extern volatile uint8_t bank;
    uint16_t *c[6] = { &table[index_table[r0] * (1 << offset)], &table[index_table[g0] * (1 << offset)], &table[index_table[b0] * (1 << offset)],
                        &table[index_table[r1] * (1 << offset)], &table[index_table[g1] * (1 << offset)], &table[index_table[b1] * (1 << offset)] };
    
    for (uint32_t i = 0; i < (1 << PWM_bits); i++) {
        uint8_t v = 0;
        for (uint8_t j = 0; j < 6; j++) {
            if (c[j][i / 16] & (1 << (i % 16)))
                v |= 1 << j;
        }
        buf[bank][y][x][i] = v;
    }
}

void work() {
    extern volatile test *p;
    extern volatile bool isReady;
    build_tables();
    while(1) {
        if (isReady) {
            for (int y = 0; y < 8; y++)
                for (int x = 0; x < 128; x++)
                    set_pixel(x, y, *p[y][x][0], *p[y][x][1], *p[y][x][2], *p[y + 8][x][0], *p[y + 8][x][1], *p[y + 8][x][2]);
        }
    }
}

