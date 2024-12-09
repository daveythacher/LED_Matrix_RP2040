/* 
 * File:   filter.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef SERIAL_PROTOCOL_SERIAL_COMMAND_FILTER_H
#define SERIAL_PROTOCOL_SERIAL_COMMAND_FILTER_H

#include "TCAM/TCAM.h"

namespace Serial::Protocol::DATA_NODE {
    class filter {
        public:
            static void filter_setup();

        private:
            static TCAM::Table<SIMD::SIMD_SINGLE<uint8_t>> data_filter;
    };
}

#endif
