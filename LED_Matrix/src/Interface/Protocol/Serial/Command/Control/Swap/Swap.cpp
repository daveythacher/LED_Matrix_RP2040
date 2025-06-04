/* 
 * File:   Swap.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "Interface/Protocol/Serial/Command/Control/Swap/Swap.h"
#include "System/machine.h"

namespace Interface::Protocol::Serial {
    Swap::Swap() : Command() {
        // Do nothing
    }

    void Swap::process_command(::Interface::Node::Node *node, uint16_t seq_num, uint8_t len) {

    }
}