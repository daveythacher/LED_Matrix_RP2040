This is just a little note to self.

In the future adding support for hardware PWM drivers can be done using the BCM nibble notion. This wastes some memory but we are not worried about that. Max size for GEN 3 is 16x64 (8S) which is smaller than 64x128 (32S) potential. Max size for GEN 2 is 16x16 (8S). Max size for GEN 1: 16x8 (8S). (I am assuming 3kHz refresh as requirement.)

Add dot correction internally. For GEN 1 on RP2040 the max size of this is 32x8 with uint8_t as integer float. (Dual HUB75)

Removing calculator function. Making HUB75 PWM hard coded to 8x8 only.


BCM nibble notion: 4-bit input expanded to 16 possible arrays of 4 bits with six possible bits shifts. 

```C
uint8_t lut[16][6][4];

for (uint8_t j = 0; j < 16 / 4; j++) {
    for (uint8_t k = 0; k < 4; k++) {
        result[k] |= lut[val >> (4 * j) & 0xF][i][k];
        result++;
    }
}
```

```C++
/* 
 * File:   Dot_Product.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "SIMD/SIMD.h"

namespace Matrix {
    template <typename T> struct Product_Result {
        public:
            T data[8];    // 8 bitplanes
    };

    // T is GPIO bank width in multiples of eight.
    // R is number of GPIO pins used times sizeof(T).
    // Note: This will be used for hardware PWM drivers in the future.
    //  These work off 16-bit inputs. (Submit two rounds.)
    //  We lose time on arbitrary width. (Note LUT may outperform SIMD!)
    //      Use specialized version SIMD implementation for R
    template <typename T, typename R> Product_Result<T> get_product(SIMD::SIMD<T, R> pixels, SIMD::SIMD<T, R> shifts) {
        SIMD::SIMD<T, R> mask;
        Product_Result<T> result;

        // TODO: Look into Matrix processor?
        for (uint8_t i = 0; i < sizeof(Product_Result<T>) / sizeof(T); i++) {
            // Create bit array extractor
            mask.load(1 << i);

            // WARNING: We have a coupling problem here.
            //  The inputs are 6xB (pixels) which are converted to Bx6xB then converted Bxn (result).
            //  This only works when n is equal to B, which originates from pixels.
            //  If n is 32 we have to translate, B to 32.
            //  Note: This problem assumes we will use a single HUB75 within a 32-bit IO bank.
            //      We would sign extend the inputs from B to 32 in the front. (Lowering throughput!)
            //      We would intentionally only compute so many bitplanes. (8/16 vs 32)
            //      The IO bank size defines the input size regardless of bitplanes.
            //  Note: R must be number of pins in the port times sizeof(T)

            // Extract bitplanes and apply shift masks using dot product to combine into single result
            result.data[i] = pixels.test_bit(mask).dot_prodoct(shifts);
        }

        return result;
    }
}
```
