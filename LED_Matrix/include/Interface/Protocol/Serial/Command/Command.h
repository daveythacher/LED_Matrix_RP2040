/* 
 * File:   Command.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef INTERFACE_PROTOCOL_SERIAL_COMMAND_H
#define INTERFACE_PROTOCOL_SERIAL_COMMAND_H

#include <stdint.h>
#include "Interface/Node/Node.h"
#include "Matrix/Matrix.h"

namespace Interface::Protocol::Serial {
    class Command {
        public:
            Command();
            
            virtual void process_command(::Interface::Node::Node *node, uint16_t seq_num, uint8_t len) = 0;

        protected:
            static ::Matrix::Packet *packet;
    };
}

#endif

