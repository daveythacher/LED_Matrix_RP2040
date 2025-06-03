/* 
 * File:   Header.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef INTERFACE_PROTOCOL_SERIAL_HEADER_H
#define INTERFACE_PROTOCOL_SERIAL_HEADER_H

#include <stdint.h>
#include "Interface/Node/Node.h"
#include "Interface/Protocol/Serial/Command/Command.h"

namespace Interface::Protocol::Serial {
    class Header {
        public:
            static Header *create_header(::Interface::Node::Node *node);

            void process_command();

        protected:
            Header();
            Header(::Interface::Node::Node *node);

        private:
            void shift();
            Command *look_up_command();
            bool compare(uint8_t index);
            uint8_t get_index(Command *command);

            static Header *ptr;
            static const uint8_t number_commands = 2;

            ::Interface::Node::Node *node;
            uint32_t data[4];

            struct TCAM {
                public:
                    uint32_t values[4];
                    uint32_t enables[4];
                    Command *command;
            };

            TCAM table[number_commands];
    };
}

#endif

