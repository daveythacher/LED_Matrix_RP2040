/* 
 * File:   DMA.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef CONCURRENT_DMA_H
#define CONCURRENT_DMA_H

#include <stdint.h>
#include "Concurrent/DMA/Target/IO_Target/IO_Target.h"
#include "Concurrent/DMA/Target/Memory_Target/Memory_Target.h"

namespace Concurrent::IO {
    // DMA is SMT in CMT cluster (SMP-1)
    //  Conceptualize as service of multi-processing.
    //  Implemented as coprocessor in background of OS.
    //      Creates a hardware or software thread.
    //  Would be nice to verify bandwidth and latency.
    class DMA {
        public:
            static DMA *acquire_DMA_channel();
            static void *release_DMA_channel(DMA *channel);
            
            // IO to IO should be wrapped by memory/CPU.
            // Memory to Memory should be handled by CPU.
            template <typename T> void submit(IO_Target<T> *src, Memory_Target<T> *dest, uint8_t priority);
            template <typename T> void submit(Memory_Target<T> *src, IO_Target<T> *dest, uint8_t priority);

            bool isFinished();
            void abort();

        protected:
            DMA();

        private:
            static DMA *_ptr[4];
            static uint8_t _flags;
    };
}

#endif
