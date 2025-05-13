/* 
 * File:   Types.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef MATRIX_TYPES_H
#define MATRIX_TYPES_H
    
#include <stdint.h>

namespace Matrix {
    struct RGBRGB {
        public:
            uint8_t get_red(bool isHigh) { return (val >> (isHigh ? 3 : 0)) & 0x1; }
            uint8_t get_green(bool isHigh) { return (val >> (isHigh ? 4 : 1)) & 0x1; }
            uint8_t get_blue(bool isHigh) { return (val >> (isHigh ? 5 : 2)) & 0x1; }

            void set_red(bool isHigh, bool on) { 
                if (isHigh) {
                    val &= ~(1 << 3);
                    val |= (on ? (1 << 3) : 0); 
                }
                else {
                    val &= ~(1 << 0);
                    val |= (on ? (1 << 0) : 0); 
                }
            }

            void set_green(bool isHigh, bool on) { 
                if (isHigh) {
                    val &= ~(1 << 4);
                    val |= (on ? (1 << 4) : 0); 
                }
                else {
                    val &= ~(1 << 1);
                    val |= (on ? (1 << 1) : 0); 
                }
            }

            void set_blue(bool isHigh, bool on) { 
                if (isHigh) {
                    val &= ~(1 << 5);
                    val |= (on ? (1 << 5) : 0); 
                }
                else {
                    val &= ~(1 << 2);
                    val |= (on ? (1 << 2) : 0); 
                }
            }
        
        private:
            uint8_t val;
    };
}

#endif
