/* 
 * File:   Multiplex.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "Multiplex/Multiplex.h"
#include "Multiplex/BUS8/Decoder/Decoder.h"
#include "Matrix/config.h"

namespace Multiplex {
    Multiplex *Multiplex::create_multiplex(int start_flag, int signal_flag) {
        return HARDWARE::MULTIPLEXER::MULTIPLEXER::create_multiplex(start_flag, signal_flag);
    }
}