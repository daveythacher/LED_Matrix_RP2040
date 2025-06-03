/* 
 * File:   Data.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef INTERFACE_PROTOCOL_SERIAL_COMMAND_DATA_DATA_H
#define INTERFACE_PROTOCOL_SERIAL_COMMAND_DATA_DATA_H

#include "Interface/Protocol/Serial/Command/Command.h"

namespace Interface::Protocol::Serial {
    class Data : public Command {
        public:
            void process_command(::Interface::Node::Node *node);
    };
}

#endif
