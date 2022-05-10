/* 
 * File:   Multiplex.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include <assert.h>
#include "hardware/gpio.h"
#include "Multiplex/Decoder.h"
#include "Multiplex/Direct.h"

Multiplex *Multiplex::ptr = nullptr;

Multiplex::Multiplex() {
    for (int i = 0; i < 5; i++) {
        gpio_init(i + 11);
        gpio_set_dir(i + 11, GPIO_OUT);
    }
}

Multiplex::~Multiplex() {
    // Do nothing
}

Multiplex *Multiplex::getMultiplexer(int num) {
    if (ptr != nullptr)
        return ptr;
        
    switch (num) {
        case 0:
            ptr = new Decoder();
            break;
        case 1:
            ptr = new Direct();
            break;
        default:
            assert(false);
    }
    
    return ptr;
}
