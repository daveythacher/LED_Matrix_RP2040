/* 
 * File:   Data.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "Interface/Protocol/Serial/Command/Data/Data/Data.h"
#include "Matrix/config.h"
#include "System/machine.h"

namespace Interface::Protocol::Serial {
    Data::Data() : Command() {
        crc = new CRC::CRC32();
    }

    void Data::process_command(::Interface::Node::Node *n, uint16_t seq_num, uint8_t len) {
        node = n;

        if (read_intermediate_header()) {
            return;     // Error
        }
    }

    bool Data::read_intermediate_header() {
        uint32_t checksum = 0;

        // Fill index, row and column
        //  Start response packet
        crc->reset();
        while (!node->get_available()) {}
        index = node->get() << 8;
        crc->process(index >> 8);
        while (!node->put_available()) {}
        node->put(index >> 8);
        while (!node->get_available()) {}
        index |= node->get();
        crc->process(index & 0xFF);
        while (!node->put_available()) {}
        node->put(index & 0xFF);
        index = ntohs(index);
        while (!node->get_available()) {}
        row = node->get();
        while (!node->put_available()) {}
        node->put(row);
        crc->process(row);
        while (!node->get_available()) {}
        column = node->get();
        while (!node->put_available()) {}
        node->put(column);
        crc->process(column);

        // Grab checksum
        for (int i = 24; i >= 0; i -= 8) {
            while (!node->get_available()) {}
            checksum |= node->get() << i;
        }

        // Send checksum
        for (int i = 24; i >= 0; i -= 8) {
            while (!node->put_available()) {}
            node->put(checksum >> i & 0xFF);
        }

        // Check for errors
        //  Send nack, if found.
        if (htonl(crc->get()) != checksum || row >= ::Matrix::MULTIPLEX || column >= ::Matrix::COLUMNS || index >= ::Matrix::STEPS) {
            crc->reset();
            crc->process(0xFF);
            checksum = htonl(crc->get());
            
            while (!node->put_available()) {}
            node->put(0xFF);    // Error Status code

            for (int i = 24; i >= 0; i -= 8) {
                while (!node->put_available()) {}
                node->put(checksum >> i & 0xFF);
            }
            return true;
        }

        return false;
    }
}