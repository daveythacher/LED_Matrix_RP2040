/* 
 * File:   ID.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef INTERFACE_PROTOCOL_SERIAL_COMMAND_DATA_ID_H
#define INTERFACE_PROTOCOL_SERIAL_COMMAND_DATA_ID_H

#include "Interface/Protocol/Serial/Command/Command.h"

namespace Interface::Protocol::Serial {
    class ID : public Command {
        public:
            void process_command(::Interface::Node::Node *node);
    };
}

#endif
