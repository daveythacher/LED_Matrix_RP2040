/* 
 * File:   Serial.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef INTERFACE_PROTOCOL_SERIAL_H
#define INTERFACE_PROTOCOL_SERIAL_H

#include <stdint.h>
#include "Interface/Protocol/Protocol.h"

namespace Interface::Protocol {
    class Serial : public ::Interface::Protocol {
        public:
            static Serial *create_protocol(Node *node);

            void work();

        protected:
            Serial();
            Serial(Node *node);

        private:
            Node *node;

            static Protocol *ptr;
    };
}

#endif

