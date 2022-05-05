/* 
 * File:   isr.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "pico/stdlib.h"
#include "hardware/irq.h"

extern void dma_isr();

void matrix_dma_isr() __attribute__ ((weak));
void serial_dma_isr() __attribute__ ((weak));
void matrix_pio_isr() __attribute__ ((weak));
void serial_pio_isr() __attribute__ ((weak));

void __not_in_flash_func(matrix_dma_isr)() {
    // Do nothing
}

void __not_in_flash_func(serial_dma_isr)() {
    // Do nothing
}

void __not_in_flash_func(matrix_pio_isr)() {
    // Do nothing
}

void __not_in_flash_func(serial_pio_isr)() {
    // Do nothing
}

void __not_in_flash_func(dma_isr)() {
    matrix_dma_isr();
    serial_dma_isr();
}

void __not_in_flash_func(pio_isr)() {
    matrix_pio_isr();
    serial_pio_isr();
}

void isr_start() {
    irq_set_exclusive_handler(DMA_IRQ_0, dma_isr);
    irq_set_priority(DMA_IRQ_0, 0);
    irq_set_enabled(DMA_IRQ_0, true);   
}

