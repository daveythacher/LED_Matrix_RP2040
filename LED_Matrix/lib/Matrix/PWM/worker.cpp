/* 
 * File:   worker.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "pico/multicore.h"
#include "Matrix/config.h"
#include "Matrix/PWM/memory_format.h"

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

static void __not_in_flash_func(set_pixel)(uint8_t x, uint8_t y, uint8_t r0, uint8_t g0, uint8_t b0, uint8_t r1, uint8_t g1, uint8_t b1) {
    extern test2 buf[];

    // SIMD Parallel Test instruction Producing 1x6 matrix of 1 or 0
    //  Multiply 1x6 matrix by 1x6 matrix of weights [ 1, 2, 4, 8, 16, 32 ]
    //  Parallel add of results or serial add of results producing *p
    // Note SIMD is only capable on smaller data sets unless you have really large SIMD instructions.
    //  In this case you only need 6-bit or 8-bit, but if you did multiple connectors this would need to increase.
    //  In order for SIMD to be useful this must do multiple instructions in less time than serial execution.
    //  RP2040 does not support SIMD.
    //  Raspberry Pi does have SIMD however cache locality usually favors BCM, which may still support SIMD or lookup.

    // Superscalar would be nice here!
    
    // Note bitplane 255 is computed as blank, this is done to create turn off display without OE
    //  This is saves performance and memory, but could corrected.
    //  This creates small accuracy problem.
    for (uint32_t i = 0; i < (1 << PWM_bits); i++) {
        uint8_t *p = &buf[bank][y][i][x + 1];
        *p = 0;
        if (i < r0)
            *p += 1;
        if (i < g0)
            *p += 2;
        if (i < b0)
            *p += 4;
        if (i < r1)
            *p += 8;
        if (i < g1)
            *p += 16;
        if (i < b1)
            *p += 32;
    }
}

void __not_in_flash_func(work)() {
    extern void isr_start_core1();
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

