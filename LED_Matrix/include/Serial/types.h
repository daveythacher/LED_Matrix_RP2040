/* 
 * File:   types.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef TYPES_H
#define TYPES_H

    #include <stdint.h>

    typedef struct {
        uint8_t red;
        uint8_t green;
        uint8_t blue;
    } RGB24;

    const uint32_t RGB24_range_high = 1 << 8;

    typedef struct {
        uint16_t red;
        uint16_t green;
        uint16_t blue;
    } RGB48;

    const uint32_t RGB48_range_high = 1 << 16;

    typedef struct {
        uint8_t red : 2;
        uint8_t green : 2;
        uint8_t blue : 2;
    } RGB_222;

    const uint32_t RGB_222_range_high = 1 << 2;

    typedef struct {
        uint16_t red : 5;
        uint16_t green : 5;
        uint16_t blue : 5;
    } RGB_555;

    const uint32_t RGB_555_range_high = 1 << 5;

#endif

