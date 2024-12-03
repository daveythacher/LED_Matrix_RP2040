/* 
 * File:   Program.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef PIO_PROGRAM_H
#define PIO_PROGRAM_H

#include "PIO/Program/ASM/ASM.h"

namespace PIO {
    class Program {
        public:
            Program(uint8_t sideset, uint8_t delay = 0, bool enable = false);

            uint8_t get_sideset_bits();
            uint8_t get_delay_bits();
            bool enable_sideset();
            void replace(ASM *instructions, uint8_t index, uint8_t len);
            void translate(uint8_t index, uint16_t *buf, uint8_t len);

        private:
            Program();

            uint8_t _sideset;
            uint8_t _delay;
            bool _enable;
            ASM _instructions[32];
    };
}

#endif
