/* 
 * File:   worker.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#include <stdint.h>
#include "pico/multicore.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"
#include "Serial/config.h"
#include "Matrix/matrix.h"

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

void __not_in_flash_func(work)() {
    const int TX = 8;
    const int RX = 9;

    gpio_init(TX);
    gpio_init(RX);
    gpio_set_dir(TX, GPIO_OUT);
    gpio_set_function(TX, GPIO_FUNC_UART);
    gpio_set_function(RX, GPIO_FUNC_UART);
    uart_init(uart1, 115200);
    
    while(1) {
        packet *p = (packet *) multicore_fifo_pop_blocking_inline();

        uart_write_blocking(uart1, (uint8_t *) p, sizeof(packet));
    }
}

void __not_in_flash_func(process)(void *arg) {
    multicore_fifo_push_blocking_inline((uint32_t) arg);
}
