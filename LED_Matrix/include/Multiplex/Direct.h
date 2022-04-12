/* 
 * File:   Direct.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef DIRECT_H
#define DIRECT_H

#include "Multiplex.h"

class Direct : public Multiplex {
    public:    
        void SetRow(int row);
};

#endif
