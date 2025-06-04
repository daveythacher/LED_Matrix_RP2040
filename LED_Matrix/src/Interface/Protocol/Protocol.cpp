/* 
 * File:   Protocol.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "Interface/Protocol/Protocol.h"
#include "Interface/config.h"
#include "Interface/Protocol/Serial/Serial.h"

namespace Interface::Protocol {
    Protocol *Protocol::create_protocol(::Interface::Node::Node *node) {
        return PROTOCOL::PROTOCOL::create_protocol(::Interface::Node::Node::create_node());
    }
}

