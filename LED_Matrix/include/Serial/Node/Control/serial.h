/* 
 * File:   serial.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef SERIAL_NODE_CONTROL_SERIAL_H
#define SERIAL_NODE_CONTROL_SERIAL_H

namespace Serial::Node::Control {
    class Control {
        public:
            static Control *get_control_node();

        protected:
            Control();

        private:
            static Control *ptr;
    };
}

#endif

