/* 
 * File:   IRQ.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef PIO_ASM_IRQ_H
#define PIO_ASM_IRQ_H

#include "PIO/ASM/ASM.h"

namespace PIO {
    class IRQ : public ASM {
        public:
            // Future: Does not support wait
            IRQ(bool set, uint8_t num);

        protected:
            IRQ();
    };
}

#endif
