/* 
 * File:   UART.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef INTERFACE_NODE_UART_H
#define INTERFACE_NODE_UART_H

#include <stdint.h>
#include "Interface/Node/Node.h"

namespace Interface::Node::UART {
    class UART : public ::Interface::Node::Node {
        public:
            static UART *create_node();

            bool put_available();
            bool get_available();
            void put_nonblocking(uint8_t c);
            uint8_t get_nonblocking();

        protected:
            UART();

        private:
            static UART *ptr;
    };
}

#endif

