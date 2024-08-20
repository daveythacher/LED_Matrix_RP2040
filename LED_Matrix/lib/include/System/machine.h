/* 
 * File:   machine.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef MACHINE_H
#define MACHINE_H
    
#include <stdint.h>
#include <machine/endian.h>

#if __BYTE_ORDER == __LITTLE_ENDIAN
    #define ntohs(x) __bswap16(x)
    #define ntohl(x) __bswap32(x)
    #define htons(x) __bswap16(x)
    #define htonl(x) __bswap32(x)
#else
    #define ntohs(x) ((uint16_t)(x))
    #define ntoh1(x) ((uint32_t)(x))
    #define htons(x) ((uint16_t)(x))
    #define hton1(x) ((uint32_t)(x))
#endif

#endif
