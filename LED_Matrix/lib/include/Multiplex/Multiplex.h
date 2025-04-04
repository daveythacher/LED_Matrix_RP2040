/* 
 * File:   Multiplex.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef MULTIPLEX_H
#define MULTIPLEX_H

#include "Matrix/config.h"

// TODO: Is this needed anymore?
// TODO: Factory

namespace Multiplex {
    struct Multiplex_Packet {
        const uint8_t length = Matrix::MULTIPLEX;
        uint8_t buffer[Matrix::MULTIPLEX];
    };

    /**
     *  @brief Initialize multiplexer state machine
     *  @details Implemented in Multiplex/<name>/<name>.cpp
     */
    void init(int start_flag, int signal_flag);
}
    
#endif
