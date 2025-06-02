/* 
 * File:   Serial.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "Interface/Protocol/Serial/Serial.h"

namespace Interface::Protocol {
    Serial *Serial::ptr = nullptr;

    Serial::Serial() {
        // Do nothing
    }

    Serial::Serial(::Interface::Node::Node *n) {
        node = n;
    }

    Serial *Serial::create_protocol(::Interface::Node::Node *node) {
        if (node != nullptr && ptr == nullptr) {
            ptr = new Serial(node);
        }

        return ptr;
    }

    void Serial::work() {
        
    }
}