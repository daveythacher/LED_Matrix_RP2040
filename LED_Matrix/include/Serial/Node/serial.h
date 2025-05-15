/* 
 * File:   serial.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef SERIAL_NODE_SERIAL_H
#define SERIAL_NODE_SERIAL_H

namespace Serial {
    class Node {
        public:
            static Node *get_node();

        protected:
            Node();

        private:
            static Node *ptr;
    };
}

#endif
