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
}

