/* 
 * File:   isr.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "pico/platform.h"
#include "hardware/irq.h"
#include "Matrix/matrix.h"

namespace APP {
    static void __not_in_flash_func(dma_isr1)() {
        Matrix::dma_isr();
    }

    void isr_start_core1() {
        irq_set_exclusive_handler(DMA_IRQ_1, dma_isr1);
        irq_set_priority(DMA_IRQ_1, 0);
        irq_set_enabled(DMA_IRQ_1, true);
    }
}
