/* 
 * File:   serial.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef SERIAL_NODE_DATA_SERIAL_H
#define SERIAL_NODE_DATA_SERIAL_H

namespace Serial::Node::Data {
    class Data {
        public:
            static Data *get_data_node();

        protected:
            Data();

        private:
            static Data *ptr;
    };
}

#endif
