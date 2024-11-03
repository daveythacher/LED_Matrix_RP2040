/* 
 * File:   SIMD_SINGLE.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef SIMD_SINGLE_H
#define SIMD_SINGLE_H

#include <stdint.h>
#include "SIMD/SIMD_QUARTER.h"

namespace SIMD {
    template <typename T> class SIMD_SINGLE {  // Quarter is 128-bit SIMD
        public:
            const SIMD_SINGLE<T> operator|(SIMD_SINGLE<T> const& arg) const;
            const SIMD_SINGLE<T> operator&(SIMD_SINGLE<T> const& arg) const;
            const bool operator==(SIMD_SINGLE<T> const& arg) const;

            static constexpr uint32_t size() {
                static_assert(sizeof(T) * 8 <= 128, "SIMD Single is limited to arguments less than 128-bits.");

                return 128 / (sizeof(T) * 8);
            }

            union {
                T v[SIMD_QUARTER<T>::size()];
                uint8_t  b[16];
                uint16_t s[8];
                uint32_t l[4];
                uint64_t ll[2];
            };
    };
}

#endif
