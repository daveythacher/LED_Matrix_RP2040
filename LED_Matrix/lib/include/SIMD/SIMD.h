#ifndef SIMD_H
#define SIMD_H

namespace SIMD {
    template <typename T> class SIMD_QUARTER {  // Quarter is 32-bit SIMD
        public:
            SIMD_QUARTER<T> operator|(SIMD_QUARTER<T> const& arg);

            union {
                uint8_t  b[4];
                uint16_t s[2];
                uint32_t l;
            };
    };
}

#endif
