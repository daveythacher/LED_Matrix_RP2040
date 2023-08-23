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
#include "Matrix/BCM/memory_format.h"

static uint8_t bank = 1;
volatile bool vsync = false;
static uint8_t index_table[1 << PWM_bits][6][PWM_bits];

// Copied from pico-sdk/src/rp2_common/pico_multicore/multicore.c
//  Allows inlining to RAM func. (Currently linker is copy-to-RAM)
//  May be better to use -ffunction-sections and -fdata-sections with custom linker script
//   I do not want to worry with custom linker script, if possible.
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

// Copied from pico-sdk/src/rp2_common/pico_multicore/multicore.c
static inline void __not_in_flash_func(multicore_fifo_push_blocking_inline)(uint32_t data) {
    // We wait for the fifo to have some space
    while (!multicore_fifo_wready())
        tight_loop_contents();

    sio_hw->fifo_wr = data;

    // Fire off an event to the other core
    __sev();
}

static uint8_t *__not_in_flash_func(get_table)(uint16_t v, uint8_t i) {
    constexpr uint32_t div = max((uint32_t) range_high / 1 << PWM_bits, (uint32_t) 1);
    constexpr uint32_t mul = max((uint32_t) 1 << PWM_bits / range_high, (uint32_t) 1);
    
    v = v * mul / div;
    //v %= (1 << PWM_bits);
    return index_table[v][i];
}

static void __not_in_flash_func(set_pixel)(uint8_t x, uint8_t y, uint16_t r0, uint16_t g0, uint16_t b0, uint16_t r1, uint16_t g1, uint16_t b1) {
    extern test2 buf[];
    uint8_t *c[6] = { get_table(r0, 0), get_table(g0, 1), get_table(b0, 2), get_table(r1, 3), get_table(g1, 4), get_table(b1, 5) };

    for (uint32_t i = 0; i < PWM_bits; i++) {
        uint8_t *p = &buf[bank][y][i][x + 1];
        *p = *c[0] + *c[1] + *c[2] + *c[3] + *c[4] + *c[5];
        for (uint32_t j = 0; j < 6; j++)
            ++c[j];
    }
}

static void build_index_table() {
    for (uint32_t i = 0; i < (1 << PWM_bits); i++)
        for (uint32_t j = 0; j < PWM_bits; j++)
            for (uint8_t k = 0; k < 6; k++)
                if (i & (1 << j))
                    index_table[i][k][j] = 1 << k;
                else
                    index_table[i][k][j] = 0;
}

void __not_in_flash_func(work)() {
    build_index_table();
    
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

