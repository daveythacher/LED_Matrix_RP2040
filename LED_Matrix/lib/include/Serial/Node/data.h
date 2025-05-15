/* 
 * File:   data.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef SERIAL_NODE_DATA_H
#define SERIAL_NODE_DATA_H

#include "Serial/Node/serial.h"
#include "Serial/config.h"

namespace Serial {
    void callback(uint8_t **buf);
    uint16_t get_len();
    bool isAvailable();
    uint8_t getc();
    void putc(uint8_t c);
    uint32_t get_packet_time_us(uint16_t packet_size);
}

#endif

