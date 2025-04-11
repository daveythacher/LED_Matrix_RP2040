/* 
 * File:   PIO_Target.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef DMA_TARGET_IO_TARGET_PIO_TARGET_H
#define DMA_TARGET_IO_TARGET_PIO_TARGET_H

#include <stdint.h>
#include "Concurrent/DMA/Target/Target.h"

namespace Concurrent::IO {
    // Abstract interface for IO
    template <typename T> class PIO_Target : public IO_Target {
        public:
            static PIO_Target *create_PIO_target(uint8_t cluster, uint8_t sm);

        protected:
            PIO_Target(uint8_t cluster, uint8_t sm);

            uint8_t get_data_request() = 0;
            void *get_ptr() = 0;    // Dirty cast here

            friend class DMA;

        private:
            uint8_t _cluster;
            uint8_t _sm;
    };
}

#endif
