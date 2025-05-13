/* 
 * File:   Memory_Target.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef CONCURRENT_DMA_TARGET_MEMORY_TARGET_H
#define CONCURRENT_DMA_TARGET_MEMORY_TARGET_H

#include <stdint.h>
#include "Concurrent/DMA/Target/Target.h"

// TODO: Create RP2040 implementation? (Convert to abstract?)

namespace Concurrent::IO {
    // Abstract interface for Memory
    //  Capable of implementing linked list (1D or 2D) or vector (1D or 2D).
    template <typename T> class Memory_Target : public Taget {
        public:
            Memory_Target(uint8_t rows, uint16_t columns);

            void add_entry(uint8_t row, uint8_t column, T *ptr, uint16_t length);
            void *get_ptr();    // Dirty cast here
        
        private:
            // This needs to be hidden!
            struct entry_node {
                uint32_t len; 
                T *data;
            };
            
            entry_node **_table;

    };
}

#endif
