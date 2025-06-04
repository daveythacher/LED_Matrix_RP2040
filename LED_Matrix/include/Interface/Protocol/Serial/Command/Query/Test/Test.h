/* 
 * File:   Test.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef INTERFACE_PROTOCOL_SERIAL_COMMAND_QUERY_TEST_H
#define INTERFACE_PROTOCOL_SERIAL_COMMAND_QUERY_TEST_H

#include "Interface/Protocol/Serial/Command/Command.h"
#include "CRC/CRC32/CRC32.h"

namespace Interface::Protocol::Serial {
    class Test : public Command {
        public:
            Test();

            void process_command(::Interface::Node::Node *node, uint16_t seq_num, uint8_t len);

        private:
            ::CRC::CRC32 *crc;
    };
}

#endif
