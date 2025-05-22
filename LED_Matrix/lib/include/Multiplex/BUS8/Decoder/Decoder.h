/* 
 * File:   Decoder.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef MULTIPLEX_BUS8_DECODER_H
#define MULTIPLEX_BUS8_DECODER_H

#include "Multiplex/Multiplex.h"

namespace Multiplex::BUS8::Decoder {
    class Decoder : public Multiplex::Multiplex {
        public:
            static Decoder *create_multiplex(int start_flag, int signal_flag);

        protected:
            Decoder(int start_flag, int signal_flag);

        private:
            static Decoder *ptr;
    }
}
    
#endif
