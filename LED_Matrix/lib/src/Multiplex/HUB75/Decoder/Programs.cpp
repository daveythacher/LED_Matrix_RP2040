/* 
 * File:   Programs.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include <algorithm>
#include "Multiplex/Programs.h"
#include "PIO/Program.h"
#include "PIO/ASM/NOP/NOP.h"
#include "PIO/ASM/OUT/OUT.h"
#include "PIO/ASM/PULL/PULL.h"
#include "PIO/ASM/JMP/JMP.h"
#include "PIO/ASM/IRQ/IRQ.h"
#include "PIO/ASM/WAIT/WAIT.h"
using namespace PIO;

namespace Multiplex {
    const auto X = Registers::X;
    const auto Y = Registers::Y;
    const auto PINS = Registers::PINS;
    const auto X_DEC = Conditions::X_DEC;
    const auto Y_DEC = Conditions::Y_DEC;
    
    uint8_t Programs::get_address_program(uint16_t *instructions, uint8_t len, int start_flag, int signal_flag) {
        uint8_t size;
        Program PMP(2);     // Warning not all behavior is supported
        ASM program[] = {   // Sidesets are cleared by default
            PULL(true),
            WAIT(Flags::IRQ, true, start_flag),                 // Wait till we are called (by Ghost) - single threaded
            OUT(PINS, 5),
            IRQ(true, signal_flag),                             // Call Ghost - multi-threaded
            JMP(0)
        };

        size = sizeof(program) / sizeof(ASM);
        size = std::min(len, size);
        PMP.replace(program, 0, size);
        PMP.translate(0, instructions, size);
        return size;
    }
}