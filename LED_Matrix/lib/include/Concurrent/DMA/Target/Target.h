/* 
 * File:   Target.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef DMA_TARGET_H
#define DMA_TARGET_H

namespace Concurrent::IO {
    // Abstract interface for Target
    template <typename T> class Target {
        protected:
            void *get_ptr(); // Dirty cast here

    };
}

#endif
