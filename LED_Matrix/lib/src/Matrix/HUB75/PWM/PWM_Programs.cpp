/* 
 * File:   PWM_Programs.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include <algorithm>
#include "Matrix/HUB75/PWM/PWM_Programs.h"
#include "PIO/Program/Program.h"
#include "PIO/Program/ASM/NOP/NOP.h"
#include "PIO/Program/ASM/OUT/OUT.h"
#include "PIO/Program/ASM/PULL/PULL.h"
#include "PIO/Program/ASM/JMP/JMP.h"
#include "PIO/Program/ASM/IRQ/IRQ.h"
#include "PIO/Program/ASM/WAIT/WAIT.h"
using IO::Registers;
using IO::Conditions;
using IO::Flags;
using IO::ASM;
using IO::Program;
using IO::OUT;
using IO::PULL;
using IO::NOP;
using IO::IRQ;
using IO::WAIT;
using IO::JMP;

// Future: Template PIO programs to use something other than uint6_t (uint8_t)

namespace Matrix {
    const auto X = Registers::X;
    const auto Y = Registers::Y;
    const auto PINS = Registers::PINS;
    const auto X_DEC = Conditions::X_DEC;
    const auto Y_DEC = Conditions::Y_DEC;
    
    // Hack to lower the ISR tick rate, accelerates by 2^PWM_bits (Improves refresh performance)
    //  Automates CLK and LAT signals with DMA and PIO to handle Software PWM of entire row
    //      Works like Hardware PWM without the high refresh
    //      This is more or less how it would work with MACHXO2 FPGA and PIC32MX using PMP.
    //          Bus performance is better with RP2040. (Lower cost due to memory, CPU, hardware integration.)
    //  OE is not used in this implementation and held to low to enable the display
    //      Last shift will disable display.
    //
    //  while (1) {
    //      counter2 = (1 << PWM_bits) - 1; LAT = 0;    // Start of frame, manually push into FIFO (data stream protocol)
    //      do {
    //          counter = COLUMNS - 1;                  // Start of payload, DMA push into FIFO (data stream protocol)
    //          do {
    //              DAT = DATA; CLK = 0;                // Payload data, DMA push into FIFO (data stream protocol)
    //              CLK = 1;                            // Automate CLK pulse
    //          } while (counter-- > 0); CLK = 0;
    //          LAT = 1;                                // Automate LAT pulse at end of payload (bitplane shift)
    //          LAT = 0;
    //      } while (counter2-- > 0);
    //  }
    uint8_t PWM_Programs::get_pmp_program(uint16_t *instructions, uint8_t len) {
        uint8_t CLK = 1 << 0;
        uint8_t LAT = 1 << 1;
        uint8_t size;
        Program PMP(2);     // Warning not all behavior is supported
        ASM program[] = {   // Sidesets are cleared by default
            PULL(true),
            OUT(X, 8),
            OUT(Y, 8),
            OUT(PINS, 6),
            JMP(Y_DEC, 3).sideset(CLK),
            NOP().sideset(LAT),
            NOP().sideset(LAT),
            NOP(),
            JMP(X_DEC, 2),
            IRQ(true, WAKE_GHOST),                              // Call Ghost - single threaded
            WAIT(Flags::IRQ, true, WAKE_PMP),                   // Wait till we are called (by Ghost) - multi-threaded
            JMP(0)
        };

        size = sizeof(program) / sizeof(ASM);
        size = std::min(len, size);
        PMP.replace(program, 0, size);
        PMP.translate(0, instructions, size);
        return size;
    }

    uint8_t PWM_Programs::get_ghost_program(uint16_t *instructions, uint8_t len) {
        uint8_t OE = 1 << 0;
        uint8_t size;
        Program PMP(1);     // Warning not all behavior is supported
        ASM program[] = {   // Sidesets are cleared by default
            PULL(true).sideset(OE),
            OUT(X, 8).sideset(OE),
            WAIT(Flags::IRQ, true, WAKE_GHOST).sideset(OE),     // Wait till we are called (by PMP) - multi-threaded
            IRQ(true, WAKE_MULTIPLEX),                          // Call Multiplex - multi-threaded
            PULL(true),
            OUT(Y, 8),
            NOP(),
            JMP(Y_DEC, 7),
            WAIT(Flags::IRQ, true, WAKE_GHOST).sideset(OE),     // Wait till we are called (by Multiplex) - single threaded
            JMP(X_DEC, 2).sideset(OE),
            IRQ(true, WAKE_PMP).sideset(OE),                    // Call PMP - multi-threaded
            JMP(0).sideset(OE)
        };

        size = sizeof(program) / sizeof(ASM);
        size = std::min(len, size);
        PMP.replace(program, 0, size);
        PMP.translate(0, instructions, size);
        return size;        
    }
}