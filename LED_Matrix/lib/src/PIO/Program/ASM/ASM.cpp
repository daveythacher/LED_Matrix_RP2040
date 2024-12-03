/* 
 * File:   ASM.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "hardware/pio_instructions.h"
#include "PIO/Program/ASM/ASM.h"
#include "PIO/Program/Program.h"

namespace PIO {
    ASM::ASM() {
        _sideset = 0;
        _delay = 0;
        _instruction = pio_encode_nop();
    }

    ASM ASM::sideset(uint8_t sideset) {
        _sideset = sideset;
        return *this;
    }

    ASM ASM::delay(uint8_t delay) {
        _delay = delay;
        return *this;
    }

    uint16_t ASM::translate(Program *program) {
        uint8_t max = program->get_sideset_bits();
        uint16_t temp = _instruction;
        
        if (program->enable_sideset())
            temp |= pio_encode_sideset_opt(max, _sideset % (1 << max));
        else
            temp |= pio_encode_sideset(max, _sideset % (1 << max));

        return temp | pio_encode_delay(_delay % (1 << program->get_delay_bits()));
    }

    void ASM::set_instruction(uint16_t val) {
        _instruction = val;
    }
}