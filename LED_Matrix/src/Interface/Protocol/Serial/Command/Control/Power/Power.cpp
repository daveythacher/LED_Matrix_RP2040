/* 
 * File:   Power.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "Interface/Protocol/Serial/Command/Control/Power/Power.h"
#include "System/machine.h"

namespace Interface::Protocol::Serial {
    Power::Power() : Command() {
        crc = new ::CRC::CRC32();
    }

    void Power::process_command(::Interface::Node::Node *node, uint16_t seq_num, uint8_t len) {
        uint32_t checksum;
        uint8_t cmd = node->get();

        crc->reset();
        crc->process(cmd);
        checksum = node->get() << 24;
        checksum |= node->get() << 16;
        checksum |= node->get() << 8;
        checksum |= node->get();
        node->put(cmd);

        // If wrong, nack
        if (htonl(crc->get()) != checksum) {
            for (uint8_t i = 0; i < 4; i++) {
                node->put(0xFF);
            }
        }
        else {
            // TODO: Use cmd

            node->put(checksum >> 24);
            node->put(checksum >> 16 & 0xFF);
            node->put(checksum >> 8 & 0xFF);
            node->put(checksum & 0xFF);
        }
    }
}