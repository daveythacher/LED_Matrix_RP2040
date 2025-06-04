/* 
 * File:   Serial.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "Interface/Protocol/Serial/Serial.h"
#include "Interface/Protocol/Serial/Header.h"

namespace Interface::Protocol::Serial {
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
        while (1) {
            Header::create_header(node)->process_command();
        }
    }
}