/* 
 * File:   Conditions.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef PIO_PROGRAM_ASM_JMP_CONDITIONS_H
#define PIO_PROGRAM_ASM_JMP_CONDITIONS_H

namespace IO {
    enum class Conditions {
        NOT_X,
        X_DEC,
        NOT_Y,
        Y_DEC,
        X_NOT_EQ_Y,
        PIN,
        NOT_OSRE
    };
}

#endif
