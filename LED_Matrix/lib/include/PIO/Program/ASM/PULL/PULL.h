/* 
 * File:   PULL.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef PIO_PROGRAM_ASM_PULL_H
#define PIO_PROGRAM_ASM_PULL_H

#include "PIO/Program/ASM/ASM.h"

namespace PIO {
    class PULL : public ASM {
        public:
            // Future: Does not support IfEmpty
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
