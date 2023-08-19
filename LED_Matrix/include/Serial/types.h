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

    typedef struct {
        uint16_t red;
        uint16_t green;
        uint16_t blue;
    } RGB48;

    typedef struct {
        uint8_t red : 2;
        uint8_t green : 2;
        uint8_t blue : 2;
    } RGB_222;

    typedef struct {
        uint16_t red : 5;
        uint16_t green : 5;
        uint16_t blue : 5;
    } RGB_555;

#endif

