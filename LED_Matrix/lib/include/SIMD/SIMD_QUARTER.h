/* 
 * File:   SIMD_QUARTER.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef SIMD_QUARTER_H
#define SIMD_QUARTER_H

#include <stdint.h>

namespace SIMD {
    template <typename T> class SIMD_QUARTER {  // Quarter is 32-bit SIMD
        public:
            const SIMD_QUARTER<T> operator|(SIMD_QUARTER<T> const& arg) const;
            const SIMD_QUARTER<T> operator&(SIMD_QUARTER<T> const& arg) const;
            const bool operator==(SIMD_QUARTER<T> const& arg) const;

            static constexpr uint32_t size() {
                static_assert(sizeof(T) * 8 <= 32, "SIMD Quarter is limited to arguments less than 32-bits.");

                return 32 / (sizeof(T) * 8);
            }

            union {
                T v[SIMD_QUARTER<T>::size()];
                uint8_t  b[4];
                uint16_t s[2];
                uint32_t l;
            };
    };
}

#endif
