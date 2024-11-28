/* 
 * File:   NOP.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "hardware/pio_instructions.h"
#include "PIO/ASM/NOP/NOP.h"

namespace PIO {
    NOP::NOP() {
        uint16_t val = pio_encode_nop();
        set_instruction(val);
    }
}