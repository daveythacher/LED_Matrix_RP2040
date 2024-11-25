#include "hardware/pio_instructions.h"
#include "PIO/ASM/ASM.h"
#include "PIO/Program.h"

namespace PIO {
    ASM::ASM() {
        _sideset = 0;
        _instruction = pio_encode_nop();
    }

    ASM ASM::sideset(uint8_t sideset) {
        _sideset = sideset;
        return *this;
    }

    uint16_t ASM::translate(Program *program) {
        uint8_t max = program->get_sideset_bits();
        return _instruction | pio_encode_sideset(max, _sideset % (1 << max)); 
    }

    void ASM::set_instruction(uint16_t val) {
        _instruction = val;
    }
}