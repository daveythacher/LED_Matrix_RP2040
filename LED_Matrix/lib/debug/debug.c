/* 
 * File:   debug.c
 * Author: David Thacher
 * License: GPL 3.0
 */

#include <pico/stdio.h>
#include "debug/debug.h"

void debug_start() {
    stdio_init_all();
}
