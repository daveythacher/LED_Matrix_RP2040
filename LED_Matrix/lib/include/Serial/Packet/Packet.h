/* 
 * File:   Packet.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef SERIAL_PACKET_H
#define SERIAL_PACKET_H

#include <stdint.h>

namespace Serial {
    class Packet {
        public:
            Packet(uint8_t MTU = 64);

            void push(uint8_t c);
            uint8_t *operator[](int id);
        
        private:
            uint8_t *vector;
            uint8_t head;
            uint8_t size;
    };
}

#endif
