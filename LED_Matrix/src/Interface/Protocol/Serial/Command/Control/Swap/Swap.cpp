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

    // We already acked and verified this command
    void Swap::process_command(::Interface::Node::Node *node, uint16_t seq_num, uint8_t len) {
        ::Matrix::Matrix::get_matrix()->show(packet);
    }
}