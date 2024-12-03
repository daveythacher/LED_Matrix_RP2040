/* 
 * File:   OUT.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef PIO_PROGRAM_ASM_OUT_H
#define PIO_PROGRAM_ASM_OUT_H

#include "PIO/Program/ASM/ASM.h"

namespace IO {
    class OUT : public ASM {
        public:
            OUT(Registers reg, uint8_t count);

        protected:
            OUT();
    };
}

#endif
