/* 
 * File:   OUT.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "hardware/pio_instructions.h"
#include "PIO/Program/ASM/OUT/OUT.h"

namespace PIO {
    OUT::OUT() {}

    OUT::OUT(Registers reg, uint8_t count) {
        uint16_t val = pio_encode_nop();

        count %= (1 << 5);

        switch (reg) {
            case Registers::X:
                val = pio_encode_out(pio_x, count);
                break;
            case Registers::Y:
                val = pio_encode_out(pio_y, count);
                break;
            case Registers::ISR:
                val = pio_encode_out(pio_isr, count);
                break;
            case Registers::OSR:
                val = pio_encode_out(pio_osr, count);
                break;
            case Registers::PINS:
                val = pio_encode_out(pio_pins, count);
                break;
            case Registers::NONE:
                val = pio_encode_out(pio_null, count);
                break;
            case Registers::PC:
                val = pio_encode_out(pio_pc, count);
                break;
            case Registers::EXEC:
                val = pio_encode_out(pio_exec_out, count);
                break;
            case Registers::PINDIRS:
                val = pio_encode_out(pio_pindirs, count);
                break;
            default:
                break;
        }

        set_instruction(val);
    }
}