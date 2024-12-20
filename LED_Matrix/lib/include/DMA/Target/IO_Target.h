/* 
 * File:   IO_Target.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef DMA_IO_TARGET_H
#define DMA_IO_TARGET_H

#include "DMA/Target/Target.h"

namespace IO {
    // Abstract interface for IO
    // T must be uint8_t, uint16_t or uint32_t
    template <typename T> class IO_Target : public Target<T> {
        public:
            IO_Target();

    };
}

#endif
