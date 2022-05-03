/* 
 * File:   config.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef CONFIG_H
#define CONFIG_H    
    
    // -- DO NOT EDIT BELOW THIS LINE --
    
    constexpr int POWER_DIVISOR = DEFINE_POWER_DIVISOR;
    constexpr int MULTIPLEX = DEFINE_MULTIPLEX;
    constexpr int MAX_RGB_LED_STEPS = DEFINE_MAX_RGB_LED_STEPS;       // Min RGB constant forward current (Blue LED in my case) in nA divided by min light current in nA
    constexpr int MAX_REFRESH = DEFINE_MAX_REFRESH;
    constexpr int COLUMNS = DEFINE_COLUMNS;
    constexpr int MULTIPLEX_NUM = DEFINE_MULTIPLEX_NUM;
    constexpr bool USE_CIE1931 = (DEFINE_USE_CIE1931 == 1);
    constexpr float SERIAL_CLOCK = (DEFINE_SERIAL_CLOCK * 1000000.0);
    constexpr int BLANK_TIME = DEFINE_BLANK_TIME;
    
    #include <math.h>
    constexpr int PWM_bits = round(log2((double) MAX_RGB_LED_STEPS / MULTIPLEX));
    
    typedef uint8_t test[2 * MULTIPLEX][COLUMNS][3];
    typedef uint8_t test2[MULTIPLEX][PWM_bits][COLUMNS];

#endif

