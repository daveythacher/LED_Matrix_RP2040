/* 
 * File:   GPIO.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef GPIO_H
#define GPIO_H

namespace IO {
    /**
     *  @brief Claim a pin forever
     *  @details Implemented in GPIO/GPIO.cpp
     */
    void claim(unsigned int pin);
}
    
#endif
