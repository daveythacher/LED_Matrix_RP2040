/* 
 * File:   isr.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "pico/platform.h"
#include "hardware/irq.h"

extern void matrix_dma_isr();
extern void serial_dma_isr();

void __not_in_flash_func(dma_isr0)() {
    matrix_dma_isr();
}

void __not_in_flash_func(dma_isr1)() {
    serial_dma_isr();
}

void isr_start() {
    irq_set_exclusive_handler(DMA_IRQ_0, dma_isr0);
    irq_set_priority(DMA_IRQ_0, 0);
    irq_set_enabled(DMA_IRQ_0, true);   
    irq_set_exclusive_handler(DMA_IRQ_1, dma_isr1);
    irq_set_priority(DMA_IRQ_1, 1 << 6);
    irq_set_enabled(DMA_IRQ_1, true);   
}

