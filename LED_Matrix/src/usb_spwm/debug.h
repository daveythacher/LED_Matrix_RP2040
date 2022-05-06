/* 
 * File:   debug.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef DEBUG_H
#define DEBUG_H

#include "pico/stdlib.h"
#include <stdio.h>

//#define debug_print

#ifdef debug_print
    #define debug(...)      printf(__VA_ARGS__)
    #define debug_start()   stdio_init_all()
#else
    #define debug(...)
    #define debug_start()
#endif

#endif
