/* 
 * File:   GPIO.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>

namespace IO {
    class GPIO {
        public:
            static void claim(unsigned int pin);

        private:
            static uint32_t claimed;
    };
}
    
#endif
