/* 
 * File:   Node.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef INTERFACE_NODE_H
#define INTERFACE_NODE_H

#include <stdint.h>

namespace Interface {
    class Node {
        public:
            static Node *create_node();

            virtual bool put_available() = 0;
            virtual bool get_available() = 0;
            virtual void put(uint8_t c) = 0;
            virtual uint8_t get() = 0;

        protected:
            Node();
    };
}

#endif

