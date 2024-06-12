/* 
 * File:   Buffer.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef SERIAL_BUFFER_H
#define SERIAL_BUFFER_H

#include <stdint.h>
#include "Serial/config.h"

namespace Serial {
    class Buffer {
        public:
            void set_value(uint8_t multiplex, uint8_t column, uint8_t value);
            void set_value(uint8_t multiplex, uint8_t column, uint16_t value);
            uint16_t get_value(uint8_t multiplex, uint8_t column);

        private:
            uint8_t buf[payload_size];
    };
}

#endif
