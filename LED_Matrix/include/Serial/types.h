/* 
 * File:   types.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef SERIAL_TYPES_H
#define SERIAL_TYPES_H
    
#include <stdint.h>

namespace Serial {
    struct RGB24 {
        volatile uint8_t red;
        volatile uint8_t green;
        volatile uint8_t blue;

        static constexpr uint32_t range_high = 1 << 8;
        static constexpr uint8_t id = 0;
    };

    struct RGB48 {
        volatile uint16_t red;
        volatile uint16_t green;
        volatile uint16_t blue;

        static constexpr uint32_t range_high = 1 << 16;
        static constexpr uint8_t id = 1;
    };

    struct RGB_222 {
        volatile uint8_t red : 2;
        volatile uint8_t green : 2;
        volatile uint8_t blue : 2;

        static constexpr uint32_t range_high = 1 << 2;
        static constexpr uint8_t id = 3;
    };

    struct RGB_555 {
        volatile uint16_t red : 5;
        volatile uint16_t green : 5;
        volatile uint16_t blue : 5;

        static constexpr uint32_t range_high = 1 << 5;
        static constexpr uint8_t id = 2;
    };
}

#endif
