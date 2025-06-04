/* 
 * File:   Node.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "Interface/Node/Node.h"
#include "Interface/config.h"
#include "Interface/Node/UART/UART.h"

namespace Interface::Node {
    Node *Node::create_node() {
        return NODE::NODE::create_node();
    }

    uint8_t Node::get() {
        while (!get_available()) {}
        return get_nonblocking();
    }

    void Node::put(uint8_t c) {
        while (!put_available()) {}
        put_nonblocking(c);
    }
}
