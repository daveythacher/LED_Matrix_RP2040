/* 
 * File:   DMA.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef DMA_H
#define DMA_H

#include <stdint.h>
#include "DMA/DMA.h"

namespace IO {
    // Abstract interface for transfers
    // T must be uint8_t, uint16_t or uint32_t
    template <typename T> class Target {

    };

    // Abstract interface for Memory
    // T must be uint8_t, uint16_t or uint32_t
    template <typename T> class Memory_Target : public Target {

    };

    // Abstract interface for IO
    // T must be uint8_t, uint16_t or uint32_t
    template <typename T> class IO_Target : public Target {

    };

    // T must be uint8_t, uint16_t or uint32_t
    template <typename T> class DMA {
        public:
            DMA();

        private:
            DMA();
    };

    // T must be uint32_t, uint64_t or uint128_t
    template <typename T> class SCATTER_GATHER_DMA {
        public:
            SCATTER_GATHER_DMA();

        private:
            SCATTER_GATHER_DMA();
    };

    // TODO: Replace with RTOS task?
    class DMA_ISR {
        public:
            static void isr();
            static bool set_isr(uint8_t i, void (*p)());

        private:
            static void (*isrs[12])();
    };
}

#endif
