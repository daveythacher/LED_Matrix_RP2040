/* 
 * File:   SIMD.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef SIMD_H
#define SIMD_H

#include <stdint.h>

namespace SIMD {
    struct SIMD_128 {
        uint8_t data[16];
    };

    struct SIMD_64 {
        uint8_t data[8];
    };

    struct SIMD_32 {
        uint8_t data[4];
    };

    struct SIMD_HUB75_Byte {
        uint8_t data[6];
    };

    template <typename T, typename R> class SIMD {
        public:
            const SIMD<T, R> operator|(SIMD<T, R> const& arg) const;
            const SIMD<T, R> operator&(SIMD<T, R> const& arg) const;
            const bool operator==(SIMD<T, R> const& arg) const;

            const T dot_prodoct(SIMD<T, R> const& arg) const;
            const SIMD<T, R> test_bit(SIMD<T, R> const& arg) const;
            const SIMD<T, R> load(T const& arg) const;

            static constexpr uint32_t size() {
                return sizeof(R) / sizeof(T);
            }

            T get(uint8_t index);
            void set(T val, uint8_t index);
        
        private:
            union {
                T v[SIMD<T, R>::size()];
                uint32_t l[sizeof(R) / sizeof(uint32_t)];
            };
    };
}

#endif
