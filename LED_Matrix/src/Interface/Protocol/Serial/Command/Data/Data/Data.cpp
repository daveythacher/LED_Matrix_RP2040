/* 
 * File:   Data.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "Interface/Protocol/Serial/Command/Data/Data/Data.h"
#include "System/machine.h"

namespace Interface::Protocol::Serial {
    Data::Data() : Command() {
        // Do nothing
    }

    void Data::process_command(::Interface::Node::Node *node, uint16_t seq_num, uint8_t len) {

    }
}