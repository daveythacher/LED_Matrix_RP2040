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

            void initialize_pin(uint8_t num, bool output);

            uint8_t set_program(Program *p, uint8_t len);
            void set_program(uint8_t index);

            void set_sticky(bool sticky);
            void set_wrap_top(uint8_t wrap);
            void set_autopull(bool autopull);
            void set_pull_thresh(uint8_t thresh);
            void set_shift_out_direction(bool right);
            void set_sideset_count(uint8_t count);
            void set_out_count(uint8_t count);
            void set_sideset_base(uint8_t base);
            void set_out_base(uint8_t base);

            void set_clock(uint16_t integer, uint16_t fraction);

            void enable(bool enable, uint8_t index);


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
