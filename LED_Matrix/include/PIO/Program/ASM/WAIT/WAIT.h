/* 
 * File:   WAIT.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef PIO_PROGRAM_ASM_WAIT_H
#define PIO_PROGRAM_ASM_WAIT_H

#include "PIO/Program/ASM/ASM.h"
#include "PIO/Program/ASM/WAIT/Flags.h"

namespace IO {
    class WAIT : public ASM {
        public:
            WAIT(Flags flag, bool set, uint8_t num);

        protected:
            WAIT();
    };
}

#endif
