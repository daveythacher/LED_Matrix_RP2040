/* 
 * File:   Serial.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef SERIAL_PROTOCOL_SERIAL_H
#define SERIAL_PROTOCOL_SERIAL_H

#include <stdint.h>

namespace Serial {
    class Protocol {
        public:
            static Protocol *create_protocol();

            virtual void work() = 0;

        protected:
            Protocol();

        private:
            static Protocol *ptr;
    };
}

#endif

