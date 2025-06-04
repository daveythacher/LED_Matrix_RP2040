/* 
 * File:   Node.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef INTERFACE_NODE_H
#define INTERFACE_NODE_H

#include <stdint.h>

namespace Interface::Node {
    class Node {
        public:
            static Node *create_node();

            virtual bool put_available() = 0;
            virtual bool get_available() = 0;
            virtual void put_nonblocking(uint8_t c) = 0;
            virtual uint8_t get_nonblocking() = 0;
            void put(uint8_t c);
            uint8_t get();
    };
}

#endif

