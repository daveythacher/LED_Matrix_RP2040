/* 
 * File:   Header.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "Interface/Protocol/Serial/Header.h"
#include "Interface/Protocol/Serial/Command/Data/Data/Data.h"
#include "Interface/Protocol/Serial/Command/Control/Power/Power.h"
#include "Interface/Protocol/Serial/Command/Control/Swap/Swap.h"
#include "Interface/Protocol/Serial/Command/Control/Reboot/Reboot.h"
#include "Interface/Protocol/Serial/Command/Query/Test/Test.h"
#include "System/machine.h"

namespace Interface::Protocol::Serial {
    Header *Header::ptr = nullptr;

    Header::Header() {
        // Do nothing
    }

    // Warning: Vulnerable to packet of death.
    //  Glitching the Marker and Checksum.
    //      Not sure this will ever matter.
    Header::Header(::Interface::Node::Node *n) {
        node = n;
        crc = new CRC::CRC32();

        for (uint8_t i = 0; i < number_words; i++) {
            data[i] = 0;
        }

        table[0].command = new Data();
        table[0].values[2] = 0x44440100;    // 'D' 'D'
        table[0].values[3] = 0x00000100;    // Type 1
        table[1].command = new Test();
        table[1].values[2] = 0x51540100;    // 'Q' 'T'
        table[1].values[3] = 0x00000100;    // Type 1
        table[2].command = new Power();
        table[2].values[2] = 0x43500100;    // 'C' 'P'
        table[2].values[3] = 0x00000100;    // Type 1
        table[3].command = new Swap();
        table[3].values[2] = 0x43530100;    // 'C' 'S'
        table[3].values[3] = 0x00000100;    // Type 1
        table[4].command = new Power();
        table[4].values[2] = 0x43520100;    // 'C' 'R'
        table[4].values[3] = 0x00000100;    // Type 1

        for (uint8_t i = 0; i < number_commands; i++) {
            table[i].enables[0] = 0xFFFFFFFF;   // MARKER[0:3]
            table[i].enables[1] = 0xFFFFFFFF;   // MARKER[4:7]
            table[i].enables[2] = 0xFFFFFFFF;   // CMD[0:1], VER[0:1]
            table[i].enables[3] = 0x0000FF00;   // SEQ_NUM[0:1], TYPE, LEN
            table[i].enables[4] = 0x00000000;   // CHECKSUM[0:3]
            table[i].values[0] = 0x52503230;    // "RP20" (MARKER[0:3])
            table[i].values[1] = 0x34302121;    // "40!!" (MARKER[4:7])
        }
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
        for (int8_t i = number_words - 1; i >= 0; i -= 1) {
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
                while (!node->get_available()) {}
                data[0] |= node->get();
            }
            else {
                // Verify checksum
                crc->reset();
                for (int8_t i = number_words - 2; i >= 0; i -= 1) {
                    for (int8_t j = 24; j >= 0; j -= 8) {
                        crc->process(data[i] >> j & 0xFF);
                    }
                }

                if (htonl(crc->get()) != data[4]) {
                    shift();
                    while (!node->get_available()) {}
                    data[0] |= node->get();
                    result = nullptr;
                }
            }

        } while (result == nullptr);

        // Write out the response header (already in network order)
        for (int8_t i = number_words - 1; i >= 0; i -= 1) {
            for (int8_t j = 24; j >= 0; j -= 8) {
                while (!node->put_available()) {}
                node->put((data[i] >> j) & 0xFF);
            }
        }

        result->process_command(node, ntohs(data[3] >> 16), data[3] & 0xFF);
    }

    void Header::shift() {
        uint32_t temp, carry = 0;

        for (uint8_t i = 0; i < number_words; i++) {
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

        for (uint8_t i = 0; i < number_words; i++) {
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