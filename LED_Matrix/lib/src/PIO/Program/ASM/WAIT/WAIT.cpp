/* 
 * File:   WAIT.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "hardware/pio_instructions.h"
#include "PIO/Program/ASM/WAIT/WAIT.h"

namespace IO {
    WAIT::WAIT() {}

    WAIT::WAIT(Flags flag, bool set, uint8_t num) {
        uint16_t val = pio_encode_nop();

        switch (flag) {
            case Flags::IRQ:
                num %= 8;
                val = pio_encode_wait_irq(set, false, num);
                break;
            case Flags::GPIO:
                num %= 1 << 5;
                val = pio_encode_wait_gpio(set, num);
                break;
            case Flags::PIN:
                num %= 1 << 5;
                val = pio_encode_wait_pin(set, num);
                break;
            default:
                break;
        }
        
        set_instruction(val);
    }
}