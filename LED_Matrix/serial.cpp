#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "config.h"

static volatile bool isReady;
static void *ptrs[2];

// Copied from pico-sdk/src/rp2_common/pico_multicore/multicore.c
//  Allows inlining to RAM func.
static inline void multicore_fifo_push_blocking_inline(uint32_t data) {
    while (!multicore_fifo_wready())
        tight_loop_contents();
    sio_hw->fifo_wr = data;
    __sev();
}

void loop() {
    ptrs[0] = malloc(sizeof(test));
    ptrs[1] = malloc(sizeof(test));
    while (1) {
        if (isReady) {
            multicore_fifo_push_blocking_inline((uint32_t) ptrs[0]);
            ptrs[0] = ptrs[1];
            ptrs[1] = malloc(sizeof(test));
            isReady = false;
        }
    }
}
