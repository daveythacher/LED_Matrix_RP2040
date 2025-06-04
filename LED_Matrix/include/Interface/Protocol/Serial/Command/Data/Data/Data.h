/* 
 * File:   Data.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef INTERFACE_PROTOCOL_SERIAL_COMMAND_DATA_DATA_H
#define INTERFACE_PROTOCOL_SERIAL_COMMAND_DATA_DATA_H

#include "Interface/Protocol/Serial/Command/Command.h"
#include "CRC/CRC32/CRC32.h"

namespace Interface::Protocol::Serial {
    class Data : public Command {
        public:
            Data();

            void process_command(::Interface::Node::Node *node, uint16_t seq_num, uint8_t len);

        private:
            bool read_intermediate_header();
            void send_error_nack(uint16_t count);

            uint16_t index;
            uint8_t row;
            uint8_t column;

            CRC::CRC32 *crc;
            ::Interface::Node::Node *node;
    };
}

#endif
