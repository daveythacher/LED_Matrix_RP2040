/* 
 * File:   SIMD_SINGLE.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "SIMD/SIMD_SINGLE.h"
#include "pico/multicore.h"

namespace SIMD {
    template <typename T> const SIMD_SINGLE<T> __not_in_flash_func(SIMD_SINGLE<T>::operator|)(SIMD_SINGLE<T> const& arg) const {
        SIMD_SINGLE<T> result;

        for (uint32_t i = 0; i < sizeof(SIMD_SINGLE<T>) / sizeof(SIMD_SINGLE<T>::l); i++) {
            result.l[i] = this->l[i] | arg.l[i];
        }

        return result;
    }

    template <typename T> const SIMD_SINGLE<T> SIMD_SINGLE<T>::operator&(SIMD_SINGLE<T> const& arg) const {
        SIMD_SINGLE<T> result;

        for (uint32_t i = 0; i < sizeof(SIMD_SINGLE<T>) / sizeof(SIMD_SINGLE<T>::l); i++) {
            result.l[i] = this->l[i] & arg.l[i];
        }
        
        return result;
    }

    template <typename T> const bool __not_in_flash_func(SIMD_SINGLE<T>::operator==)(SIMD_SINGLE<T> const& arg) const {
        bool result = true;
        
        for (uint32_t i = 0; i < SIMD_SINGLE<T>::size(); i++) {
            result &= this->v[i] == arg.v[i];
        }

        return result;
    }

    template class SIMD_SINGLE<uint32_t>;
}