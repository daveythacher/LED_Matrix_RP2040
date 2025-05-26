/* 
 * File:   machine.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef SYSTEM_MACHINE_H
#define SYSTEM_MACHINE_H
    
#include <stdint.h>

uint16_t ntohs(uint16_t val);
uint16_t htons(uint16_t val);
uint32_t ntohl(uint32_t val);
uint32_t htonl(uint32_t val);

#endif
