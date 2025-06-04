/* 
 * File:   Power.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "Interface/Protocol/Serial/Command/Control/Power/Power.h"
#include "System/machine.h"

namespace Interface::Protocol::Serial {
    Power::Power() : Command() {
        // Do nothing
    }

    void Power::process_command(::Interface::Node::Node *node, uint16_t seq_num, uint8_t len) {

    }
}