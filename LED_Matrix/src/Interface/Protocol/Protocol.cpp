/* 
 * File:   Protocol.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "Interface/Protocol/Protocol.h"
#include "Interface/config.h"
#include "Interface/Protocol/Serial/Serial.h"
#include "Interface/Node/UART/UART.h"

namespace Interface {
    Protocol *Protocol::create_protocol(Node *node) {
        return Protocol::PROTOCOL::create_protocol(Node::NODE::create_node());
    }
}

