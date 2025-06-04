/* 
 * File:   CRC32.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef CRC_CRC32_H
#define CRC_CRC32_H
    
#include <stdint.h>

namespace CRC {
    class CRC32 {
        public:
            CRC32();

            void reset();
            void process(uint8_t data);
            uint32_t get();
        
        private:
            uint32_t checksum;
    };
}

#endif
