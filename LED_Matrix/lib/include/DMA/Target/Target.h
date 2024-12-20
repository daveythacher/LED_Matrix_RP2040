/* 
 * File:   Target.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef DMA_TARGET_H
#define DMA_TARGET_H

namespace IO {
    // Abstract interface for transfers
    // T must be uint8_t, uint16_t or uint32_t
    template <typename T> class Target {
        public:
            Target();
    };
}

#endif
