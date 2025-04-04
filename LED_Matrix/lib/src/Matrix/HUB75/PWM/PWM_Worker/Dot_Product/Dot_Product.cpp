/* 
 * File:   Dot_Product.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "SIMD/SIMD.h"

namespace Matrix {
    // Inputs: 6 4-bit array of bitplanes and 6 shift masks
    // Outputs: 4 6-bit bitplanes 
    template <typename T> SIMD::SIMD<uint8_t, SIMD::SIMD_HUB75_Byte> get_product(SIMD::SIMD<uint8_t, SIMD::SIMD_HUB75_Byte> pixels, SIMD::SIMD<uint8_t, SIMD::SIMD_HUB75_Byte> shifts) {
        SIMD::SIMD<uint8_t, SIMD::SIMD_HUB75_Byte> result, mask;

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
