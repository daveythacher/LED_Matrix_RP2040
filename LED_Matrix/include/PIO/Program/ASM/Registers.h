/* 
 * File:   Registers.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef PIO_PROGRAM_ASM_REGISTERS_H
#define PIO_PROGRAM_ASM_REGISTERS_H

namespace IO {
    enum class Registers {
        X,
        Y,
        ISR,
        OSR,
        PC,
        NONE,
        EXEC,
        PINS,
        PINDIRS
    };
}

#endif
