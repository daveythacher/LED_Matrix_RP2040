/* 
 * File:   matrix.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#include "pico/multicore.h"
#include "Matrix/matrix.h"

volatile int timer;

void matrix_start() {
    // Do nothing

    timer = hardware_alarm_claim_unused(true);
}

void __not_in_flash_func(matrix_dma_isr)() {
    // Do nothing
}

void __not_in_flash_func(matrix_timer_isr)() {
    // Do nothing
}
