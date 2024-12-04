/* 
 * File:   SM.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "PIO/SM/SM.h"
#include "GPIO/GPIO.h"

namespace IO {
    SM::SM() {}

    SM::SM(int pio) {
        if (pio == 0) {
            data.pio = 0;
            data.sm = pio_claim_unused_sm(pio0, true);
        }
        else {
            data.pio = 1;
            data.sm = pio_claim_unused_sm(pio1, true);
        }
    }

    void SM::initialize_pin(uint8_t pin, bool output) {
        uint32_t val = output ? 1 : 0;
        pin %= 30;

        claim(pin);
        gpio_init(pin);
        gpio_set_dir(pin, output);

        if (data.pio == 0) {
            gpio_set_function(pin, GPIO_FUNC_PIO0);
            pio_sm_set_pindirs_with_mask(pio0, data.sm, val << pin, 1 << pin);
        }
        else {
            gpio_set_function(pin, GPIO_FUNC_PIO1);
            pio_sm_set_pindirs_with_mask(pio1, data.sm, val << pin, 1 << pin);
        }
    }

    void SM::set_program(Program *p, uint8_t len) {
        uint16_t instr[len];

        static const struct pio_program pio_programs = {
            .instructions = instr,
            .length = len,
            .origin = 0,
        };

        enable(false);
        p->translate(0, instr, len);

        if (data.pio == 0) {
            pio0->sm[data.sm].instr = pio_add_program(pio0, &pio_programs);
        }
        else {
            pio1->sm[data.sm].instr = pio_add_program(pio1, &pio_programs);
        }
    }

    void SM::set_program(uint8_t index) {
        enable(false);

        if (data.pio == 0) {
            pio0->sm[data.sm].instr = index;
        }
        else {
            pio1->sm[data.sm].instr = index;
        }
    }

    void SM::enable(bool enable) {
        if (enable) {
            if (data.pio == 0)
                hw_set_bits(&pio0->ctrl, 1 << (PIO_CTRL_SM_ENABLE_LSB + data.sm));
            else
                hw_set_bits(&pio1->ctrl, 1 << (PIO_CTRL_SM_ENABLE_LSB + data.sm));
        }
        else {
            if (data.pio == 0)
                hw_clear_bits(&pio0->ctrl, 1 << (PIO_CTRL_SM_ENABLE_LSB + data.sm));
            else
                hw_clear_bits(&pio1->ctrl, 1 << (PIO_CTRL_SM_ENABLE_LSB + data.sm));
        }
    }
}