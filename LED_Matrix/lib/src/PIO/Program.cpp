#include "PIO/Program.h"
#include "PIO/ASM/NOP/NOP.h"

namespace PIO {
    Program::Program() {}

    Program::Program(uint8_t sideset) {
        _sideset = 0;

        if (sideset > 0 && sideset < 6)
            _sideset = sideset;

        for (uint8_t i = 0; i < 32; i++)
            _instructions[i] = NOP();
    }

    uint8_t Program::get_sideset_bits() {
        return _sideset;
    }

    void Program::replace(ASM *instructions, uint8_t index, uint8_t len) {
        for (uint8_t i = 0; i < len; i++) {
            uint8_t j = (index + i) % 32;
            _instructions[j] = instructions[i];
        }
    }

    void Program::translate(uint8_t index, uint16_t *buf, uint8_t len) {
        for (uint8_t i = 0; i < len; i++) {
            uint8_t j = (index + i) % 32;
            buf[i] = _instructions[j].translate(this);
        }
    }
}