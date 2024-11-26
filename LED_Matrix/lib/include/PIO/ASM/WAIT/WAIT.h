/* 
 * File:   WAIT.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef PIO_ASM_WAIT_H
#define PIO_ASM_WAIT_H

#include "PIO/ASM/ASM.h"

namespace PIO {
    enum class Flags {
        GPIO,
        IRQ,
        PIN
    };

    class WAIT : public ASM {
        public:
            WAIT(Flags flag, bool set, uint8_t num);

        protected:
            WAIT();
    };
}

#endif
