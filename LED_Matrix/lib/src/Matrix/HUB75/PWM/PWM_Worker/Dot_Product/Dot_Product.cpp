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

/*
// 64-bit input: (8 iteration of i, 8 call of the above function, 8 channels in parallel)
                                                                [ 1,   1,   1,   1,   1,   1,   1,   1   ]
                                                                [ 2,   2,   2,   2,   2,   2,   2,   2   ]
                                                                [ 4,   4,   4,   4,   4,   4,   4,   4   ]
[ r0'bi, g0'bi, b0'bi, r1'bi, g1'bi, b1'bi, x0'bi, x1'bi ]  *   [ 8,   8,   8,   8,   8,   8,   8,   8   ]  =   [ a0'bi, a1'bi, a2'bi, a3'bi, a4'bi, a5'bi, a6'bi, a7'bi ]
                                                                [ 16,  16,  16,  16,  16,  16,  16,  16  ]
                                                                [ 32,  32,  32,  32,  32,  32,  32,  32  ]
                                                                [ 64,  64,  64,  64,  64,  64,  64,  64  ]
                                                                [ 128, 128, 128, 128, 128, 128, 128, 128 ]
*/