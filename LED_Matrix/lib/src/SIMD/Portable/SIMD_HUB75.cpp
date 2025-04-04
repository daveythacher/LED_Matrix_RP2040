/* 
 * File:   SIMD_HUB75.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "SIMD/SIMD_HUB75.h"
#include "pico/multicore.h"

namespace SIMD {
    template <typename T> const SIMD_HUB75<T> __not_in_flash_func(SIMD_HUB75<T>::operator|)(SIMD_HUB75<T> const& arg) const {
        SIMD_HUB75<T> result;

        for (uint32_t i = 0; i < SIMD_HUB75<T>::size(); i++) {
            result.v[i] = this->v[i] | arg.v[i];
        }

        return result;
    }

    template <typename T> const SIMD_HUB75<T> SIMD_HUB75<T>::operator&(SIMD_HUB75<T> const& arg) const {
        SIMD_HUB75<T> result;

        for (uint32_t i = 0; i < SIMD_HUB75<T>::size(); i++) {
            result.v[i] = this->v[i] & arg.v[i];
        }
        
        return result;
    }

    template <typename T> const bool SIMD_HUB75<T>::operator==(SIMD_HUB75<T> const& arg) const {
        bool result = true;
        
        for (uint32_t i = 0; i < SIMD_HUB75<T>::size(); i++) {
            result &= this->v[i] == arg.v[i];
        }

        return result;
    }

    template <typename T> const T SIMD_HUB75<T>::dot_prodoct(SIMD_HUB75<T> const& arg) const {
        T result = 0;

        for (uint32_t i = 0; i < SIMD_HUB75<T>::size(); i++) {
            result += this->v[i] * arg.v[i];
        }
        
        return result;
    }

    template <typename T> const SIMD_HUB75<T> SIMD_HUB75<T>::test_bit(SIMD_HUB75<T> const& arg) const {
        SIMD_HUB75<T> result;

        for (uint32_t i = 0; i < SIMD_HUB75<T>::size(); i++) {
            result.v[i] = this->v[i] & arg.v[i] ? 1 : 0;
        }
        
        return result;
    }

    template <typename T> const SIMD_HUB75<T> SIMD_HUB75<T>::load(T const& arg) const {
        SIMD_HUB75<T> result;

        for (uint32_t i = 0; i < SIMD_HUB75<T>::size(); i++) {
            result.v[i] = arg;
        }
        
        return result;
    }

    template <typename T> T SIMD_HUB75<T>::get(uint8_t index) {
        return v[index % size()];
    }

    template <typename T> void SIMD_HUB75<T>::set(T val, uint8_t index) {
        v[index % size()] = val;
    }

    template class SIMD_HUB75<uint8_t>;
}