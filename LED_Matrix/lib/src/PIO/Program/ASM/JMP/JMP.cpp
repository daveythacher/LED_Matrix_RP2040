/* 
 * File:   JMP.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "hardware/pio_instructions.h"
#include "PIO/Program/ASM/JMP/JMP.h"

namespace PIO {
    JMP::JMP() {}

    JMP::JMP(uint8_t addr) {
        addr %= (1 << 5);
        uint16_t val = pio_encode_jmp(addr);
        set_instruction(val);
    }

    JMP::JMP(Conditions cond, uint8_t addr) {
        uint16_t val = pio_encode_nop();
        addr %= (1 << 5);

        switch (cond) {
            case Conditions::NOT_OSRE:
                val = pio_encode_jmp_not_osre(addr);
                break;
            case Conditions::NOT_X:
                val = pio_encode_jmp_not_x(addr);
                break;
            case Conditions::NOT_Y:
                val = pio_encode_jmp_not_y(addr);
                break;
            case Conditions::X_DEC:
                val = pio_encode_jmp_x_dec(addr);
                break;
            case Conditions::Y_DEC:
                val = pio_encode_jmp_y_dec(addr);
                break;
            case Conditions::X_NOT_EQ_Y:
                val = pio_encode_jmp_x_ne_y(addr);
                break;
            case Conditions::PIN:
                val = pio_encode_jmp_pin(addr);
                break;
            default:
                break;
        }

        set_instruction(val);
    }
}