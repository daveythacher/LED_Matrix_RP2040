/* 
 * File:   serial.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef SERIAL_PROTOCOL_SERIAL_H
#define SERIAL_PROTOCOL_SERIAL_H

#include <stdint.h>

namespace Serial::Protocol {
    class Protocol {
        public:
            static Protocol *create_protocol();

        protected:
            Protocol();

        private:
            static Protocol *ptr;
    };
}

#endif

