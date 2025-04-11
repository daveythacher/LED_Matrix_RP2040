/* 
 * File:   DMA.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef DMA_H
#define DMA_H

#include <stdint.h>
#include "Concurrent/DMA/Target/IO_Target.h"
#include "Concurrent/DMA/Target/Memory_Target.h"

namespace Concurrent::IO {
    // DMA is SMT in CMT cluster (SMP-1)
    //  Conceptualize as service of multi-processing.
    //  Implemented as coprocessor in background of OS.
    //      Multithreading is external wrapper to this.
    class DMA {
        public:
            static DMA *acquire_DMA_channel();
            static void *release_DMA_channel(DMA *channel);
            
            // IO to IO should be wrapped by memory/CPU.
            // Memory to Memory should be handled by CPU.
            void submit(IO_Target *src, Memory_Target *dest, uint32_t us);
            void submit(Memory_Target *src, IO_Target *dest, uint32_t us);

        protected:
            DMA();

        private:
            static DMA *_ptr[4];
            static uint8_t _flags;
    };
}

#endif
