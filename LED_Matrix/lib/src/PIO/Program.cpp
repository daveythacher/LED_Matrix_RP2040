#include "PIO/Program.h"
#include "PIO/ASM/NOP/NOP.h"

namespace PIO {
    Program::Program() {}

    Program::Program(uint8_t sideset, uint8_t delay, bool enable) {
        _sideset = 0;
        _delay = 0;
        _enable = enable;

        if (_enable) {
            if (delay > 0 && delay < 5) {
                _delay = delay;
            }

            if (sideset > 0 && sideset < (5 - _delay)) {
                _sideset = sideset;
            }
        }
        else {
            if (delay > 0 && delay < 6) {
                _delay = delay;
            }

            if (sideset > 0 && sideset < (6 - _delay)) {
                _sideset = sideset;
            }
        }

        for (uint8_t i = 0; i < 32; i++)
            _instructions[i] = NOP();
    }

    uint8_t Program::get_sideset_bits() {
        return _sideset;
    }

    uint8_t Program::get_delay_bits() {
        return _delay;
    }

    bool Program::enable_sideset() {
        return _enable;
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