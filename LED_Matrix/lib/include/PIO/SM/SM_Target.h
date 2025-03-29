/* 
 * File:   SM_Target.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef PIO_SM_SM_TARGET_H
#define PIO_SM_SM_TARGET_H

#include "PIO/SM/SM.h"
#include "DMA/Target/IO_Target.h"

namespace IO {
    template <typename T> class SM_Target : public SM, IO_Target<T> {
        public:
            SM_Target(int pio);
    };
}

#endif
