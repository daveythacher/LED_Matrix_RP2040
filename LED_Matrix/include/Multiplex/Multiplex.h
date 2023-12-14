/* 
 * File:   Multiplex.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef MULTIPLEX_H
#define MULTIPLEX_H

namespace Matrix {
    // TODO: Create enum for type

    /**
     *  @brief Initialize multiplexer state machine
     *  @details Implemented in Multiplex/<name>/<name>.cpp
     */
    void init(int rows, int type);

    /**
     *  @brief Change the row for multiplexing
     *  @details Implemented in Multiplex/<name>/<name>.cpp
     */
    void SetRow(int row);
}
    
#endif
