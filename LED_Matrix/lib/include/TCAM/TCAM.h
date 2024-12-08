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
            Table(uint8_t size);
            ~Table();

            // Only the highest priority rule match runs
            bool TCAM_rule(uint8_t priority, T key, T enable, Handler *callback);
            void TCAM_process(const T *data);
        
        protected:
            Table();
            bool TCAM_search(const T *data, const T *key, const T *enable);

        private:
            uint8_t num_rules;
            T *masks;
            T *values;
            Handler **callbacks;
    };
}

#endif