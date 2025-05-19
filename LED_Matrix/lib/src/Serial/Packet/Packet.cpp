/* 
 * File:   Packet.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "Serial/Packet/Packet.h"

namespace Serial {
    Packet::Packet(uint8_t MTU) {
        vector = new uint8_t(MTU);
        head = 0;
        size = MTU;
    }

    void Packet::push(uint8_t c) {
        vector[head] = c;
        head = ++head % size;
    }
    
    uint8_t *Packet::operator[](int id) {
        if (id < 0 || id >= size) {
            return nullptr;
        }
        else {
            id = head - size - 1 + id;
            id += id < 0 ? size : 0;
            return &vector[id];
        }
    }
}

// Fill packet too MTU first.
// Access TCAM fields via external getters/enums
// Check TCAM