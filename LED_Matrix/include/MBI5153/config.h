/* 
 * File:   config.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef CONFIG_H
#define CONFIG_H    
    
    const int POWER_DIVISOR = DEFINE_POWER_DIVISOR;
    const int MULTIPLEX = DEFINE_MULTIPLEX;
    const int MAX_RGB_LED_STEPS = DEFINE_MAX_RGB_LED_STEPS;       // Min RGB constant forward current (Blue LED in my case) in nA divided by min light current in nA
    const int MAX_REFRESH = DEFINE_MAX_REFRESH;
    const int FPS = DEFINE_FPS;
    const int COLUMNS = DEFINE_COLUMNS;
    const int MULTIPLEX_NUM = DEFINE_MULTIPLEX_NUM;
    const bool USE_CIE1931 = (DEFINE_USE_CIE1931 == 1);
    
    // -- DO NOT EDIT BELOW THIS LINE --
    
    #include <math.h>
    const int PWM_bits = round(log2((double) MAX_RGB_LED_STEPS / MULTIPLEX));
    const int lower = std::min((int) round(log2(MAX_REFRESH / FPS)), PWM_bits);
    const int upper = PWM_bits - lower;
    
    typedef uint8_t test[2 * MULTIPLEX][COLUMNS][3];
    typedef uint16_t test2[6][MULTIPLEX][COLUMNS];

#endif
