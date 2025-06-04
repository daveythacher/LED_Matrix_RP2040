/* 
 * File:   Test.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "Interface/Protocol/Serial/Command/Query/Test/Test.h"
#include "Matrix/config.h"
#include "System/machine.h"

namespace Interface::Protocol::Serial {
    Test::Test() : Command() {
        crc = new ::CRC::CRC32();
    }

    void Test::process_command(::Interface::Node::Node *node, uint16_t seq_num, uint8_t len) {
        uint32_t checksum;
        uint16_t index = htons(::Matrix::STEPS);

        crc->reset();
        node->put(index >> 8);
        crc->process(index >> 8);
        node->put(index & 0xFF);
        crc->process(index & 0xFF);
        node->put(::Matrix::MULTIPLEX);
        crc->process(::Matrix::MULTIPLEX);
        node->put(::Matrix::COLUMNS);
        crc->process(::Matrix::COLUMNS);
        checksum = htonl(crc->get());

        node->put(checksum >> 24);
        node->put(checksum >> 16);
        node->put(checksum >> 8);
        node->put(checksum & 0xFF);
    }
}