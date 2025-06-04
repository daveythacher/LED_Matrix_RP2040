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
        uint32_t checksum = 0;
        uint8_t val;
        int16_t length = len - 8;
        node = n;

        // If malformed, nack intermediate header
        if (length <= 0) {
            send_error_nack(8);
            return;
        }

        // Load intermediate data and nack if malformed.
        if (read_intermediate_header()) {
            return;
        }

        // Grab pixel data and place into packet
        crc->reset();
        for (; length > 4; length--) {
            val = node->get();
            packet->set(row, index, column, val);
            crc->process(val);
            node->put(val);

            ++column;
            if (column > ::Matrix::COLUMNS) {
                ++index;
                column = 0;
                if (index > ::Matrix::STEPS) {
                    ++row;
                    index = 0;
                    if (row > ::Matrix::MULTIPLEX) {
                        row = 0;
                    }
                }
            }
        }

        // Grab checksum
        for (int i = 24; i >= 0; i -= 8) {
            checksum |= node->get() << i;
        }

        // If checksum malformed, nack payload
        if (ntohl(checksum) != crc->get()) {
            send_error_nack(4);
            return;
        }
        else {
            // Accept payload
            for (int i = 24; i >= 0; i -= 8) {
                node->put(checksum >> i & 0xFF);
            }
        }        
    }

    bool Data::read_intermediate_header() {
        uint32_t checksum = 0;

        // Fill index, row and column
        //  Start response packet
        crc->reset();
        index = node->get() << 8;
        crc->process(index >> 8);
        node->put(index >> 8);
        index |= node->get();
        crc->process(index & 0xFF);
        node->put(index & 0xFF);
        index = ntohs(index);
        row = node->get();
        node->put(row);
        crc->process(row);
        column = node->get();
        node->put(column);
        crc->process(column);

        // Grab checksum
        for (int i = 24; i >= 0; i -= 8) {
            checksum |= node->get() << i;
        }

        // Check for errors
        //  Send nack, if found.
        if (htonl(crc->get()) != checksum || row >= ::Matrix::MULTIPLEX || column >= ::Matrix::COLUMNS || index >= ::Matrix::STEPS) {
            send_error_nack(4);
            return true;
        }

        // Send checksum
        for (int i = 24; i >= 0; i -= 8) {
            node->put(checksum >> i & 0xFF);
        }

        return false;
    }

    void Data::send_error_nack(uint16_t count) {
        for (uint8_t i = 0; i < count; i++) {
            node->put(0xFF);
        }
    }
}