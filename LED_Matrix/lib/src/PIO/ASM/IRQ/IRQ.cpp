#include "hardware/pio_instructions.h"
#include "PIO/ASM/IRQ/IRQ.h"

namespace PIO {
    IRQ::IRQ() {}

    IRQ::IRQ(bool set, uint8_t num) {
        uint16_t val;

        num %= 8;
        
        if (set)
            val = pio_encode_irq_set(false, num);
        else
            val = pio_encode_irq_wait(false, num);
        
        set_instruction(val);
    }
}