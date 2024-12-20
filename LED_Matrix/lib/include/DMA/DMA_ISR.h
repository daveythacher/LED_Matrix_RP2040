/* 
 * File:   DMA_ISR.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef DMA_ISR_H
#define DMA_ISR_H

#include <stdint.h>

namespace IO {
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
