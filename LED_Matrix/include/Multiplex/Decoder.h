/* 
 * File:   Decoder.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef DECODER_H
#define DECODER_H

#include "Multiplex.h"

class Decoder : public Multiplex {
    public:    
        void SetRow(int row);
};

#endif
