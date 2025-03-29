/* 
 * File:   IRQ.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef PIO_PROGRAM_ASM_IRQ_H
#define PIO_PROGRAM_ASM_IRQ_H

#include "PIO/Program/ASM/ASM.h"

namespace IO {
    class IRQ : public ASM {
        public:
            // Future: Does not support wait
            IRQ(bool set, uint8_t num);

        protected:
            IRQ();
    };
}

#endif
