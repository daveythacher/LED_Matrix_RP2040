#include <stdint.h>
#include "pico/multicore.h"
#include "config.h"
#include "serial.h"

static volatile bool isReady;
static test buffers[2];
static uint8_t buffer = 0;

void serial_start() {
    extern void work();
    multicore_launch_core1(work);
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

void __not_in_flash_func(serial_task)() {
    if (isReady) {
        multicore_fifo_push_blocking_inline((uint32_t) &buffers[(buffer + 1) % 2]);
        isReady = false;
    }
}

