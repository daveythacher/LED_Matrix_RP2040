/* 
 * File:   Header.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "Interface/Protocol/Serial/Header.h"
#include "System/machine.h"

namespace Interface::Protocol::Serial {
    Header *Header::ptr = nullptr;

    Header::Header() {
        // Do nothing
    }

    Header::Header(::Interface::Node::Node *n) {
        node = n;

        for (uint8_t i = 0; i < 4; i++) {
            data[i] = 0;
        }

        // TODO: Setup filter
        //  Figure out type
    }

    Header *Header::create_header(::Interface::Node::Node *node) {
        if (node != nullptr && ptr == nullptr) {
            ptr = new Header(node);
        }

        return ptr;
    }

    void Header::process_command() {
        Command *result;

        // Fill header
        for (uint8_t i = 3; i >= 0; i -= 1) {
            for (int8_t j = 24; j >= 0; j -= 8) {
                while (!node->get_available()) {}
                data[i] |= node->get() << j;
            }
        }

        // TCAM for Command processor
        do {
            result = look_up_command();

            if (result == nullptr) {
                shift();
                data[0] |= node->get();
            }

        } while (result != nullptr);

        // Write out the response header (already in network order)
        for (uint8_t i = 3; i >= 0; i -= 1) {
            for (int8_t j = 24; j >= 0; j -= 8) {
                while (!node->put_available()) {}
                node->put((data[i] >> j) & 0xFF);
            }
        }

        result->process_command(node, ntohs(data[3] >> 16), data[3] & 0xFF);
    }

    void Header::shift() {
        uint32_t temp, carry = 0;

        for (uint8_t i = 0; i < 4; i++) {
            temp = carry;
            carry = data[i] >> 24;
            data[i] <<= 8;
            data[i] += temp;
        }
    }

    Command *Header::look_up_command() {
        for (uint8_t i = 0; i < number_commands; i++) {
            if (compare(i)) {
                return table[i].command;
            }
        }

        return nullptr;
    }

    bool Header::compare(uint8_t index) {
        if (index >= number_commands) {
            return false;
        }

        for (uint8_t i = 0; i < 4; i++) {
            if ((data[i] & table[index].enables[i]) != table[index].values[i]) {
                return false;
            }
        }

        return true;
    }

    uint8_t Header::get_index(Command *command) {
        for (uint8_t i = 0; i < number_commands; i++) {
            if (table[i].command == command) {
                return i;
            }
        }

        return number_commands;
    }
}