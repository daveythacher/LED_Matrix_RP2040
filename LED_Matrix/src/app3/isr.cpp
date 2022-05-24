/* 
 * File:   isr.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "pico/platform.h"
#include "hardware/irq.h"

extern void matrix_pio_isr0();
extern void matrix_pio_isr1();

void isr_start() {
    irq_set_exclusive_handler(PIO1_IRQ_0, matrix_pio_isr1);
    irq_set_priority(PIO1_IRQ_0, 0);
    irq_set_enabled(PIO1_IRQ_0, true);   
    irq_set_exclusive_handler(PIO1_IRQ_1, matrix_pio_isr0);
    irq_set_priority(PIO1_IRQ_1, 1 << 6);
    irq_set_enabled(PIO1_IRQ_1, true); 
}

