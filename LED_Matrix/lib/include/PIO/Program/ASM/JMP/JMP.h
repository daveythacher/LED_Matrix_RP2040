/* 
 * File:   JMP.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef PIO_PROGRAM_ASM_JMP_H
#define PIO_PROGRAM_ASM_JMP_H

#include "PIO/Program/ASM/ASM.h"

namespace PIO {
    enum class Conditions {
        NOT_X,
        X_DEC,
        NOT_Y,
        Y_DEC,
        X_NOT_EQ_Y,
        PIN,
        NOT_OSRE
    };

    class JMP : public ASM {
        public:
            JMP(uint8_t addr);
            JMP(Conditions cond, uint8_t addr);

        protected:
            JMP();
    };
}

#endif
