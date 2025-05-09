/* 
 * File:   IRQ.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "hardware/pio_instructions.h"
#include "PIO/Program/ASM/IRQ/IRQ.h"

namespace IO {
    IRQ::IRQ() {}

    IRQ::IRQ(bool set, uint8_t num) {
        uint16_t val;

        num %= 8;
        
        if (set)
            val = pio_encode_irq_set(false, num);
        else
            val = pio_encode_irq_clear(false, num);
        
        set_instruction(val);
    }
}