/* 
 * File:   GPIO.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "GPIO/GPIO.h"
#include "hardware/gpio.h"
#include "hardware/claim.h"

namespace IO {
    uint32_t GPIO::claimed;

    void GPIO::claim(unsigned int pin) {   // SDK did not do this!
        check_gpio_param(pin);
        hw_claim_or_assert((uint8_t *) &claimed, pin, "GPIO %d already claimed"); // SDK did this!
    }
}