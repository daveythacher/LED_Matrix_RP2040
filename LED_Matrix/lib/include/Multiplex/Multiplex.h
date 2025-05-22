/* 
 * File:   Multiplex.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef MULTIPLEX_H
#define MULTIPLEX_H

#include <stdint.h>

namespace Multiplex {
    class Multiplex {
        public:
            static Multiplex *create_multiplex(int start_flag, int signal_flag);
    };
}

#endif
