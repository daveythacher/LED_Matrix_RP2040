/* 
 * File:   config.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef CONFIG_H
#define CONFIG_H    
    
    // -- DO NOT EDIT BELOW THIS LINE --
    
    #include <algorithm>
    
    constexpr int MULTIPLEX = DEFINE_MULTIPLEX;
    constexpr int MAX_RGB_LED_STEPS = DEFINE_MAX_RGB_LED_STEPS;       // Min RGB constant forward current (Blue LED in my case) in uA divided by min light current in uA
    constexpr int MAX_REFRESH = DEFINE_MAX_REFRESH;
    constexpr int COLUMNS = DEFINE_COLUMNS;
    constexpr int MULTIPLEX_NUM = DEFINE_MULTIPLEX_NUM;
    constexpr float SERIAL_CLOCK = (DEFINE_SERIAL_CLOCK * 1000000.0);
    constexpr int BLANK_TIME = DEFINE_BLANK_TIME;
    constexpr float RED_GAMMA = DEFINE_RED_GAMMA;
    constexpr float GREEN_GAMMA = DEFINE_GREEN_GAMMA;
    constexpr float BLUE_GAMMA = DEFINE_BLUE_GAMMA;
    
    #include <math.h>
    constexpr int PWM_bits = std::min(round(log2((double) MAX_RGB_LED_STEPS / MULTIPLEX)), 8.0);
    
    typedef uint8_t test[2 * MULTIPLEX][COLUMNS][3];
    typedef uint8_t test2[MULTIPLEX][PWM_bits][COLUMNS];

#endif

