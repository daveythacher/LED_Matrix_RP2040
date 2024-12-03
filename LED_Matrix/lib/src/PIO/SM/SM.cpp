/* 
 * File:   SM.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "PIO/SM/SM.h"

namespace IO {
    SM::SM() {}

    SM::SM(int pio) {
        if (pio == 0) {
            data.initialized = 1;
            data.pio = 0;
            data.sm = pio_claim_unused_sm(pio0, true);
            data.enabled = 0;
        }
        else {
            data.initialized = 1;
            data.pio = 1;
            data.sm = pio_claim_unused_sm(pio1, true);
            data.enabled = 0;
        }
    }

    SM::~SM() {
        if (data.pio == 0)
            pio_sm_unclaim(pio0, data.sm);
        else
            pio_sm_unclaim(pio1, data.sm);
    }

    void SM::initialize_pin(uint8_t pin, bool output) {
        if (data.initialized) {
            uint32_t val = output ? 1 : 0;
            pin %= 30;

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
    }
}