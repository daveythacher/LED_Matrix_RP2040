/* 
 * File:   Multiplex.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef MULTIPLEX_H
#define MULTIPLEX_H

// TODO: Is this needed anymore?
// TODO: Factory

namespace Multiplex {
    /**
     *  @brief Initialize multiplexer state machine
     *  @details Implemented in Multiplex/<name>/<name>.cpp
     */
    void init(int start_flag, int signal_flag);
}
    
#endif
