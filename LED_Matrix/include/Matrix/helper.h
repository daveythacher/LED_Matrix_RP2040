/* 
 * File:   helper.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef HELPER_H
#define HELPER_H

#include <stdint.h>
#include <pico/multicore.h>

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

#endif

