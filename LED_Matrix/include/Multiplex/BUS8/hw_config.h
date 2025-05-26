/* 
 * File:   hw_config.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef MULTIPLEX_BUS8_HW_CONFIG_H
#define MULTIPLEX_BUS8_HW_CONFIG_H

#include <stdint.h>
#include "Matrix/config.h"

namespace Multiplex::BUS8 {
    struct Multiplex_Packet {
        const uint8_t length = Matrix::MULTIPLEX;
        uint8_t buffer[Matrix::MULTIPLEX];
    };

    constexpr uint16_t BUS8_ADDR_BASE = 18;

    // Order (LSB to MSB): A0 A1 A2
    constexpr uint16_t BUS8_ADDR_LEN = 3;
}

#endif
