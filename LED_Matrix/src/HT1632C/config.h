/* 
 * File:   config.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef CONFIG_H
#define CONFIG_H    
    
    // -- DO NOT EDIT BELOW THIS LINE --
    
    constexpr int POWER_DIVISOR = DEFINE_POWER_DIVISOR;
    constexpr int COLUMNS = DEFINE_COLUMNS;
    constexpr float SERIAL_CLOCK = (DEFINE_SERIAL_CLOCK * 1000000.0);
    
    typedef uint8_t test[16][COLUMNS][2];

#endif

