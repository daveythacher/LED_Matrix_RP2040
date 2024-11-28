/* 
 * File:   Default.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "Multiplex/Mapper/Mapper.h"

namespace Multiplex {
    Multiplex_Packet map() {
        Multiplex_Packet packet;

        for (uint8_t i = 0; i < packet.length; i++) {
            packet.buffer[i] = i;
        }

        return packet;
    }
}
