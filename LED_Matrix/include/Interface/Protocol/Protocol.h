/* 
 * File:   Protocol.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef INTERFACE_PROTOCOL_H
#define INTERFACE_PROTOCOL_H

#include <stdint.h>
#include "Interface/Node/Node.h"

namespace Interface::Protocol {
    class Protocol {
        public:
            static Protocol *create_protocol(::Interface::Node::Node *node);

            virtual void work() = 0;
    };
}

#endif

