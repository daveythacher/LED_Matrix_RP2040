/* 
 * File:   config.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef SERIAL_CONFIG_H
#define SERIAL_CONFIG_H    
    
    // -- DO NOT EDIT BELOW THIS LINE --

    #include <stdint.h>
    #include "../Matrix/config.h"
    #include "Serial/types.h"
    
    typedef DEFINE_SERIAL_RGB_TYPE test[2 * MULTIPLEX][COLUMNS];
    
    typedef union {
        test data;
        uint8_t mem[((sizeof(test) / 64) + 1) * 64];
    } packet;

    // Bad practice
    #define STR(s) s##_range_high
    #define GET_RANGE_HIGH(TYPE) STR(TYPE)

    const uint32_t range_high = GET_RANGE_HIGH(DEFINE_SERIAL_RGB_TYPE);
#endif

