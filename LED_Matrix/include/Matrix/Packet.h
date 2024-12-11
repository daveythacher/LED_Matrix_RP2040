/* 
 * File:   Packet.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef MATRIX_PACKET_H
#define MATRIX_PACKET_H

#include <stdint.h>

namespace Matrix {
    // These are used for raw bit data.
    class Packet {
        public:
            Packet();
            
            void ntoh();

        private:
            uint8_t buffer[16 * 1024];
    };
}

#endif

