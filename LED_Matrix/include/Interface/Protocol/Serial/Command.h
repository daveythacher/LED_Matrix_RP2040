/* 
 * File:   Command.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef INTERFACE_PROTOCOL_SERIAL_COMMAND_H
#define INTERFACE_PROTOCOL_SERIAL_COMMAND_H

#include <stdint.h>
#include "Interface/Node/Node.h"

namespace Interface::Protocol::Serial {
    class Command {
        public:
            virtual void process_command(::Interface::Node::Node *node) = 0;
    };
}

#endif

