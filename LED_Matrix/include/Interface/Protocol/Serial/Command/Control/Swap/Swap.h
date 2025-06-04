/* 
 * File:   Swap.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef INTERFACE_PROTOCOL_SERIAL_COMMAND_CONTROL_SWAP_H
#define INTERFACE_PROTOCOL_SERIAL_COMMAND_CONTROL_SWAP_H

#include "Interface/Protocol/Serial/Command/Command.h"

namespace Interface::Protocol::Serial {
    class Swap : public Command {
        public:
            Swap();

            void process_command(::Interface::Node::Node *node, uint16_t seq_num, uint8_t len);
    };
}

#endif
