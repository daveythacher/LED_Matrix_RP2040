/* 
 * File:   IO_Target.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef DMA_TARGET_IO_TARGET_H
#define DMA_TARGET_IO_TARGET_H

#include <stdint.h>
#include "Concurrent/DMA/Target/Target.h"

namespace Concurrent::IO {
    // Abstract interface for IO
    template <typename T> class IO_Target : public Target {        
        protected:
            uint8_t get_data_request() = 0;
            void *get_ptr() = 0;    // Dirty cast here

            friend class DMA;
    };
}

#endif
