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
            static TCAM::Table<SIMD::SIMD<uint8_t, SIMD::SIMD_128>> data_filter;
    };
}

#endif
