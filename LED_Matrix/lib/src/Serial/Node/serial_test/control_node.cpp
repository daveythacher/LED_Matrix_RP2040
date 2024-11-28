/* 
 * File:   control_node.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#include <stdint.h>
#include "pico/multicore.h"
#include "Serial/Node/Control/serial.h"

namespace Serial::Node::Control {
    void __not_in_flash_func(task)() {
        // Do nothing
    }

    void start() {
        // Do nothing
    }

    bool __not_in_flash_func(isAvailable)() {
        return false;
    }

    uint8_t __not_in_flash_func(getc)() {
        return 0;
    }
}
