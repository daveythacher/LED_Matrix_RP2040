/* 
 * File:   SM.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef PIO_SM_H
#define PIO_SM_H

#include <stdint.h>
#include "PIO/Program/Program.h"

namespace IO {
    // Future: Add destructor (Program is resource intensive for resource management.)
    class SM {
        public:
            SM(int pio);

            void set_program(Program *p, uint8_t len);
            void set_program(uint8_t index);
            void set_clock(uint16_t integer, uint16_t fraction);
            void enable(bool enable);
            void initialize_pin(uint8_t num, bool output);


        protected:
            SM();

            struct SM_fields {
                uint8_t pio : 1;
                uint8_t sm : 2;
            };

            SM_fields data;
    };
}

#endif
