/* 
 * File:   Test.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef INTERFACE_PROTOCOL_SERIAL_COMMAND_QUERY_TEST_H
#define INTERFACE_PROTOCOL_SERIAL_COMMAND_QUERY_TEST_H

#include "Interface/Protocol/Serial/Command/Command.h"

namespace Interface::Protocol::Serial {
    class Test : public Command {
        public:
            void process_command(::Interface::Node::Node *node);
    };
}

#endif
