/* 
 * File:   control.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef SERIAL_NODE_CONTROL_H
#define SERIAL_NODE_CONTROL_H

#include <stdint.h>
#include "Serial/Node/Control/serial.h"

namespace Serial::Node::Control {
    bool isAvailable();
    uint8_t getc();
}

#endif

