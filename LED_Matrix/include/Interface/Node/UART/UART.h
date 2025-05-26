/* 
 * File:   UART.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef INTERFACE_NODE_UART_H
#define INTERFACE_NODE_UART_H

#include <stdint.h>
#include "Interface/Node.h"

namespace Interface::Node {
    class UART : public ::Interface::Node {
        public:
            static UART *create_node();

            bool available();
            void put(uint8_t c);
            uint8_t get();

        protected:
            UART();

        private:
            static UART *ptr;
    };
}

#endif

