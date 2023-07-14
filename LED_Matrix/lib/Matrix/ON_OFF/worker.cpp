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

static uint8_t bank = 1;
volatile bool vsync = false;

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
                set_pixel(x, y, p->data[y][x][0], p->data[y][x][1], p->data[y][x][2], p->data[y + MULTIPLEX][x][0], p->data[y + MULTIPLEX][x][1], p->data[y + MULTIPLEX][x][2]);
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

