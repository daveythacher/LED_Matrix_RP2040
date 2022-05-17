/* 
 * File:   matrix.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#include <stdint.h>
#include <string.h>
#include "pico/platform.h"
#include "hardware/pio.h"
#include "hardware/gpio.h"
#include "hardware/dma.h"
#include "config.h"

void matrix_dma_isr();

void matrix_start() {
    // TODO:
}

void __not_in_flash_func(matrix_dma_isr)() {
    // TODO:
}

