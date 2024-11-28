/* 
 * File:   PULL.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "hardware/pio_instructions.h"
#include "PIO/ASM/PULL/PULL.h"

namespace PIO {
    PULL::PULL() {}

    PULL::PULL(bool block) {
        uint16_t val = pio_encode_pull(false, block);
        set_instruction(val);
    }
}