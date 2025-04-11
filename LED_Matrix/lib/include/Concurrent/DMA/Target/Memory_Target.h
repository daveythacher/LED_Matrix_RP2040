/* 
 * File:   Memory_Target.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef DMA_MEMORY_TARGET_H
#define DMA_MEMORY_TARGET_H

#include "DMA/Target/Target.h"

namespace Concurrent::IO {
    // Abstract interface for Memory
    class Memory_Target : public Target {
        public:
            Memory_Target();

    };
}

#endif
