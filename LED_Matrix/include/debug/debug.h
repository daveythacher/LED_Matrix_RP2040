/* 
 * File:   debug.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>

#define debug(...)      printf(__VA_ARGS__)

void debug_start();

#endif
