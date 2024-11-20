#include <machine/endian.h>
#include "System/machine.h"

uint16_t ntohs(uint16_t val) {
    #if __BYTE_ORDER == __LITTLE_ENDIAN
        return __bswap16(val);
    #else
        return val;
    #endif
}

uint16_t htons(uint16_t val) {
    #if __BYTE_ORDER == __LITTLE_ENDIAN
        return __bswap16(val);
    #else
        return val;
    #endif
}

uint32_t ntohl(uint32_t val) {
    #if __BYTE_ORDER == __LITTLE_ENDIAN
        return __bswap32(val);
    #else
        return val;
    #endif
}

uint32_t htonl(uint32_t val) {
    #if __BYTE_ORDER == __LITTLE_ENDIAN
        return __bswap32(val);
    #else
        return val;
    #endif
}