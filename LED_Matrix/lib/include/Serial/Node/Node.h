/* 
 * File:   data.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef SERIAL_NODE_DATA_H
#define SERIAL_NODE_DATA_H

#include <stdint.h>

namespace Serial {
    // TODO: Fix this
    void init();
    bool isAvailable();
    uint8_t getc();
    void putc(uint8_t c);
}

#endif

