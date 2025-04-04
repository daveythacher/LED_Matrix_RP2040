/* 
 * File:   SIMD_HUB75.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef SIMD_HUB75_H
#define SIMD_HUB75_H

#include <stdint.h>

namespace SIMD {
    template <typename T> class SIMD_HUB75 {  // HUB75 is 48-bit SIMD
        public:
            const SIMD_HUB75<T> operator|(SIMD_HUB75<T> const& arg) const;
            const SIMD_HUB75<T> operator&(SIMD_HUB75<T> const& arg) const;
            const bool operator==(SIMD_HUB75<T> const& arg) const;

            static constexpr uint32_t size() {
                static_assert(sizeof(T) * 8 <= 48, "SIMD HUB75 is limited to arguments less than 48-bits.");

                return 128 / (sizeof(T) * 8);
            }

            T get(uint8_t index);
            void set(T val, uint8_t index);
        
        private:
            union {
                T v[SIMD_HUB75<T>::size()];
                uint8_t l[6];
            };
    };
}

#endif
