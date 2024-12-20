/* 
 * File:   Memory_Target.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef DMA_MEMORY_TARGET_H
#define DMA_MEMORY_TARGET_H

#include "DMA/Target/Target.h"

namespace IO {
    // Abstract interface for Memory
    // T must be uint8_t, uint16_t or uint32_t
    template <typename T> class Memory_Target : public Target<T> {
        public:
            Memory_Target();

    };
}

#endif
