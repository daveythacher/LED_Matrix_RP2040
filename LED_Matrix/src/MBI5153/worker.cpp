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
        if (USE_CIE1931)
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

static void __not_in_flash_func(set_pixel)(uint8_t x, uint8_t y, uint8_t r0, uint8_t g0, uint8_t b0, uint8_t r1, uint8_t g1, uint8_t b1) {
    buf[bank][0][y][x] = index_table[r0];
    buf[bank][1][y][x] = index_table[g0];
    buf[bank][2][y][x] = index_table[b0];
    buf[bank][3][y][x] = index_table[r1];
    buf[bank][4][y][x] = index_table[g1];
    buf[bank][5][y][x] = index_table[b1];
}

void __not_in_flash_func(work)() {
    build_table_pwm(PWM_bits);
    memset(buf, 0, sizeof(buf));
}

