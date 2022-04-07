#ifndef CONFIG_H
#define CONFIG_H    

    #define POE_DIVISOR         4
    #define MULTIPLEX           16
    #define MAX_RGB_LED_STEPS   (2000 / 1)          // Min RGB current (Blue LED in my case) in nA divided by min light current in nA
    #define MAX_REFRESH         3840
    #define FPS                 30
    #define COLUMNS             128
    
    #include <math.h>
    const int PWM_bits = round(log2((double) MAX_RGB_LED_STEPS / (POE_DIVISOR * MULTIPLEX)));
    const int lower = std::min((int) round(log2(MAX_REFRESH / FPS)), PWM_bits);
    const int upper = PWM_bits - lower;
    const int offset = std::max(PWM_bits - 4, 0);
    
    typedef uint8_t test[2 * MULTIPLEX][COLUMNS][3];
    typedef uint8_t test2[MULTIPLEX][1 << PWM_bits][COLUMNS];

#endif
