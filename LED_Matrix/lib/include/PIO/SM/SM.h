/* 
 * File:   SM.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef PIO_SM_H
#define PIO_SM_H

#include <stdint.h>
#include "PIO/Program/Program.h"
#include "DMA/DMA.h"

namespace IO {
    class SM {
        public:
            SM(int pio);

            bool set_program(Program *p);
            void set_clock(uint16_t integer, uint16_t fraction);
            void enable(bool enable);


        protected:
            SM();

            struct SM_fields {
                uint8_t pio : 1;
                uint8_t sm : 2;
                uint8_t enabled : 1;
                uint8_t initialized : 1;
            };

            SM_fields data;
    };

    template <typename T> class SM_Target : public SM, IO_Target {
        public:
            SM_Target(int pio);
    };
}

#endif
