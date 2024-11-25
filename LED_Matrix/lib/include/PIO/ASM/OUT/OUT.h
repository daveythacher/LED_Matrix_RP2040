/* 
 * File:   OUT.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef PIO_ASM_OUT_H
#define PIO_ASM_OUT_H

#include "PIO/ASM/ASM.h"

namespace PIO {
    class OUT : public ASM {
        public:
            OUT(Registers reg, uint8_t count);

        protected:
            OUT();
    };
}

#endif
