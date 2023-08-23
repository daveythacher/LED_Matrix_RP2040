/* 
 * File:   types.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef TYPES_H
#define TYPES_H

    #include <stdint.h>

    struct RGB24 {
        uint8_t red;
        uint8_t green;
        uint8_t blue;

        static constexpr  uint32_t range_high = 1 << 8;
    };

    struct RGB48 {
        uint16_t red;
        uint16_t green;
        uint16_t blue;

        static constexpr  uint32_t range_high = 1 << 16;
    };

    struct RGB_222 {
        uint8_t red : 2;
        uint8_t green : 2;
        uint8_t blue : 2;

        static constexpr  uint32_t range_high = 1 << 2;
    };

    struct RGB_555 {
        uint16_t red : 5;
        uint16_t green : 5;
        uint16_t blue : 5;

        static constexpr uint32_t range_high = 1 << 5;
    };

#endif

