/* 
 * File:   Target.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef CONCURRENT_DMA_TARGET_H
#define CONCURRENT_DMA_TARGET_H

namespace Concurrent::IO {
    // Abstract interface for Target
    template <typename T> class Target {
        public:
            virtual void *get_ptr() = 0;    // Dirty cast here
    };
}

#endif
