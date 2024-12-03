/* 
 * File:   ASM.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef PIO_PROGRAM_ASM_H
#define PIO_PROGRAM_ASM_H

#include <stdint.h>

namespace PIO {
    class Program;

    enum class Registers {
        X,
        Y,
        ISR,
        OSR,
        PC,
        NONE,
        EXEC,
        PINS,
        PINDIRS
    };

    // Future: Missing support for IN, PUSH, MOV and SET
    class ASM {
        public:
            ASM();

            // You know copy constructors are amazing
            ASM sideset(uint8_t sideset);
            ASM delay(uint8_t delay);
            uint16_t translate(Program *program);
        
        protected:
            void set_instruction(uint16_t val);

        private:
            uint8_t _sideset;
            uint8_t _delay;
            uint16_t _instruction;
    };
}

#endif
