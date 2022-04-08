#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "config.h"

static volatile bool isReady;
static test buffers[2];
static uint8_t buffer = 0;

static void setup() {
    // TODO:
}


// Copied from pico-sdk/src/rp2_common/pico_multicore/multicore.c
//  Allows inlining to RAM func.
static inline void multicore_fifo_push_blocking_inline(uint32_t data) {
    while (!multicore_fifo_wready())
        tight_loop_contents();
    sio_hw->fifo_wr = data;
    __sev();
}

void __not_in_flash_func(loop)() {
    setup();
    while (1) {
        if (isReady) {
            multicore_fifo_push_blocking_inline((uint32_t) &buffers[(buffer + 1) % 2]);
            isReady = false;
        }
    }
}

