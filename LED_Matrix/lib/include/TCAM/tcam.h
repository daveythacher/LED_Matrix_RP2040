/* 
 * File:   tcam.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef TCAM_H
#define TCAM_H

#include <stdint.h>
#include "SIMD/SIMD_SINGLE.h"

// This can be rendered into coprocessor.
namespace TCAM {
    class Handler {
        public:
            virtual void callback() = 0;
    };

    template <typename T> class Table {
        public:
            // Only the highest priority rule match runs
            bool TCAM_rule(uint8_t priority, T key, T enable, Handler *callback);
            void TCAM_process(const T *data);
        
        private:
            // Future: Add banks (Probably not really a good idea anymore)
            static const uint8_t num_rules = 4;

            T masks[num_rules];
            T values[num_rules];
            Handler *callbacks[num_rules];
    };

    class Bank {
        public:
            // TODO:

        private:
            Table<SIMD::SIMD_SINGLE<uint32_t>> banks[2];
    };
}

#endif