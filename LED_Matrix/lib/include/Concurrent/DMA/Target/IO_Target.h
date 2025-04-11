/* 
 * File:   IO_Target.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef DMA_IO_TARGET_H
#define DMA_IO_TARGET_H

#include "DMA/Target/Target.h"

namespace Concurrent::IO {
    // Abstract interface for IO
    class IO_Target : public Target {
        public:
            IO_Target();

    };
}

#endif
