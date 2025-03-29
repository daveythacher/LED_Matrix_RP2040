/* 
 * File:   DMA.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef DMA_H
#define DMA_H

namespace Concurrent::IO {
    // DMA is SMT in CMT cluster (SMP-1)
    //  Conceptualize as service of multi-processing.

    // T must be uint8_t, uint16_t or uint32_t
    template <typename T> class DMA {
        public:
            DMA();
            
            virtual bool wait(uint8_t timeout_us) = 0;
            virtual void join() = 0;
    };
}

#endif
