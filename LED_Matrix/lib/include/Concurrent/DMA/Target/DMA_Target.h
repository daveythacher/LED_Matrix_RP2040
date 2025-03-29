/* 
 * File:   DMA_Target.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef DMA_DMA_TARGET_H
#define DMA_DMA_TARGET_H

#include "DMA/Target/IO_Target.h"

namespace Concurrent::IO {
    // Abstract interface for IO
    // T must be uint8_t, uint16_t or uint32_t
    template <typename T> class DMA_Target : public IO_Target<T> {
        public:
            DMA_Target();

    };
}

#endif
