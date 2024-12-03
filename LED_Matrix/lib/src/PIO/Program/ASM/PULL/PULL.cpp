/* 
 * File:   PULL.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "hardware/pio_instructions.h"
#include "PIO/Program/ASM/PULL/PULL.h"

namespace IO {
    PULL::PULL() {}

    PULL::PULL(bool block) {
        uint16_t val = pio_encode_pull(false, block);
        set_instruction(val);
    }
}