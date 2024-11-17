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
        public:
            uint8_t get_red() { return red; }
            uint8_t get_green() { return green; }
            uint8_t get_blue() { return blue; }
            void set_red(uint16_t v) { red = v & 0xFF; }
            void set_green(uint16_t v) { green = v & 0xFF; }
            void set_blue(uint16_t v) { blue = v & 0xFF; }

            static constexpr uint32_t range_high = 1 << 8;
            static constexpr uint8_t id = 0;

        private:
            uint8_t red;
            uint8_t green;
            uint8_t blue;
    };

    struct RGB48 {
        public:
            uint16_t get_red() { return red; }
            uint16_t get_green() { return green; }
            uint16_t get_blue() { return blue; }
            void set_red(uint16_t v) { red = v; }
            void set_green(uint16_t v) { green = v; }
            void set_blue(uint16_t v) { blue = v; }


            static constexpr uint32_t range_high = 1 << 16;
            static constexpr uint8_t id = 1;

        private:
            uint16_t red;
            uint16_t green;
            uint16_t blue;
    };

    struct RGB_222 {
        public:
            uint8_t get_red() { return val & 0x3; }
            uint8_t get_green() { return (val >> 2) & 0x3; }
            uint8_t get_blue() { return (val >> 4) & 0x3; }

            void set_red(uint16_t v) { 
                val &= ~0x3;
                val |= v & 0x3; 
            }

            void set_green(uint16_t v) { 
                val &= ~(0x3 << 2);
                val |= (v & 0x3) << 2; 
            }

            void set_blue(uint16_t v) { 
                val &= ~(0x3 << 4);
                val |= (v & 0x3) << 4; 
            }


            static constexpr uint32_t range_high = 1 << 2;
            static constexpr uint8_t id = 3;
        
        private:
            uint8_t val;
    };

    struct RGB_555 {
        public:
            uint8_t get_red() { return val & 0x1F; }
            uint8_t get_green() { return (val >> 5) & 0x1F; }
            uint8_t get_blue() { return (val >> 10) & 0x1F; }

            void set_red(uint16_t v) { 
                val &= ~0x1F;
                val |= v & 0x1F; 
            }

            void set_green(uint16_t v) { 
                val &= ~(0x1F << 5);
                val |= (v & 0x1F) << 5; 
            }

            void set_blue(uint16_t v) { 
                val &= ~(0x1F << 10);
                val |= (v & 0x1F) << 10; 
            }

            static constexpr uint32_t range_high = 1 << 5;
            static constexpr uint8_t id = 2;
        
        private:
            uint16_t val;
    };
}

#endif
