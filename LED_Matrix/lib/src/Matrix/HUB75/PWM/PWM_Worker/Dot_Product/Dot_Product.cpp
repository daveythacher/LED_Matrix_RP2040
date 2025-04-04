/* 
 * File:   Dot_Product.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "SIMD/SIMD_HUB75.h"

namespace Matrix {
    // Inputs: 6 4-bit array of bitplanes and 6 shift masks
    // Outputs: 4 6-bit bitplanes 
    template <typename T> SIMD::SIMD_HUB75<T> get_product(SIMD::SIMD_HUB75<T> pixels, SIMD::SIMD_HUB75<T> shifts) {
        SIMD::SIMD_HUB75<T> result, mask;

        for (uint8_t i = 0; i < 4; i++) {
            // Create bit array extractor
            mask.load(1 << i);

            // Extract bitplanes and apply shift masks using dot product to combine into single result
            //  TODO: Look into Matrix processor?
            result->set(pixels.test_bit(mask).dot_prodoct(shifts), i)
        }

        return result;
    }
}
