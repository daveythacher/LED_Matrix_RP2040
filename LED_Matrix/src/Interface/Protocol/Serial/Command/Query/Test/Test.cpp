/* 
 * File:   Test.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "Interface/Protocol/Serial/Command/Query/Test/Test.h"
#include "System/machine.h"

namespace Interface::Protocol::Serial {
    Test::Test() : Command() {
        // Do nothing
    }

    void Test::process_command(::Interface::Node::Node *node, uint16_t seq_num, uint8_t len) {

    }
}