/* 
 * File:   PULL.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef PIO_ASM_PULL_H
#define PIO_ASM_PULL_H

#include "PIO/ASM/ASM.h"

namespace PIO {
    class PULL : public ASM {
        public:
            PULL(bool block);

        protected:
            PULL();
    };
}

#endif

/* Note about the barf in the datasheet (3.4.7.2)
    bool pull_if_available(uint32_t *word);
    uint32_t pull(bool isBlocking);

    #define autopull pull(true)
*/