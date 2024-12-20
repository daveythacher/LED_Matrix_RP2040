/* 
 * File:   SCATTER_GATHER_DMA.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef SCATTER_GATHER_DMA_H
#define SCATTER_GATHER_DMA_H

namespace IO {
    // T must be uint32_t, uint64_t or uint128_t
    template <typename T> class SCATTER_GATHER_DMA {
        public:
            SCATTER_GATHER_DMA();
    };
}

#endif
