/* 
 * File:   types.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef TYPES_H
#define TYPES_H
    
#include <stdint.h>

namespace Serial {
    struct RGB24 {
        volatile uint8_t red;
        volatile uint8_t green;
        volatile uint8_t blue;

        static constexpr  uint32_t range_high = 1 << 8;
    };

    struct RGB48 {
        volatile uint16_t red;
        volatile uint16_t green;
        volatile uint16_t blue;

        static constexpr  uint32_t range_high = 1 << 16;
    };

    struct RGB_222 {
        volatile uint8_t red : 2;
        volatile uint8_t green : 2;
        volatile uint8_t blue : 2;

        static constexpr  uint32_t range_high = 1 << 2;
    };

    struct RGB_555 {
        volatile uint16_t red : 5;
        volatile uint16_t green : 5;
        volatile uint16_t blue : 5;

        static constexpr uint32_t range_high = 1 << 5;
    };

    // This is terrible practice!!!
    namespace internal {
        struct uart {
            static constexpr uint8_t num_framebuffers = 3;
            static constexpr uint32_t max_framebuffer_size = 48 * 1024;
            static constexpr bool isPacket = true;      // Only use Worker APIs!

            // These are never used if isPacket is false.
            static constexpr uint32_t payload_size = 12 * 1024;
            static constexpr uint8_t num_packets = 3;   // TODO: Determine is if this can be 2
        };

        struct test {
            static constexpr uint8_t num_framebuffers = 3;
            static constexpr uint32_t max_framebuffer_size = 48 * 1024;
            static constexpr bool isPacket = true;      // Only use Worker APIs!

            // These are never used if isPacket is false.
            static constexpr uint32_t payload_size = 12 * 1024;
            static constexpr uint8_t num_packets = 3;   // TODO: Determine is if this can be 2
        };
    }
}

#endif
