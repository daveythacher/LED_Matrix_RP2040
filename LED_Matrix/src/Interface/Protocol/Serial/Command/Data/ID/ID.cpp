/* 
 * File:   ID.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "Interface/Protocol/Serial/Command/Data/ID/ID.h"
#include "System/machine.h"

namespace Interface::Protocol::Serial {
    ID::ID() : Command() {
        // Do nothing
    }

    void ID::process_command(::Interface::Node::Node *node, uint16_t seq_num, uint8_t len) {

    }
}