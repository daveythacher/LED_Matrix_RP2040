/* 
 * File:   serial.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef SERIAL_PROTOCOL_SERIAL_H
#define SERIAL_PROTOCOL_SERIAL_H

#include <stdint.h>

namespace Serial::Protocol {
    class Packet {
        public:
            uint8_t *serialize();
            void deserialize(uint8_t *buf, uint16_t len);

            static Packet *create() {
                return new(std::align_val_t(sizeof(uint32_t))) Packet();
            }

        private:
            alignas(uint32_t) uint8_t buffer[8 * 1024];
    };

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

