/* 
 * File:   SIMD.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "SIMD/SIMD.h"

namespace SIMD {
    template <typename T, typename R> const SIMD<T, R> SIMD<T, R>::operator|(SIMD<T, R> const& arg) const {
        SIMD<T, R> result;

        switch (sizeof(R)) {
            case 4:
                result.l[0] = this->l[0] | arg.l[0];
                break;
            case 16:
                for (uint32_t i = 0; i < 4; i++) {
                    result.l[i] = this->l[i] | arg.l[i];
                }
                break;
            default:
                for (uint32_t i = 0; i < size(); i++) {
                    result.v[i] = this->v[i] | arg.v[i];
                }
                break;
        }

        return result;
    }

    template <typename T, typename R> const SIMD<T, R> SIMD<T, R>::operator&(SIMD<T, R> const& arg) const {
        SIMD<T, R> result;

        switch (sizeof(R)) {
            case 4:
                result.l[0] = this->l[0] & arg.l[0];
                break;
            case 16:
                for (uint32_t i = 0; i < 4; i++) {
                    result.l[i] = this->l[i] & arg.l[i];
                }
                break;
            default:
                for (uint32_t i = 0; i < size(); i++) {
                    result.v[i] = this->v[i] & arg.v[i];
                }
                break;
        }
        
        return result;
    }

    template <typename T, typename R> const bool SIMD<T, R>::operator==(SIMD<T, R> const& arg) const {
        bool result = true;
        
        for (uint32_t i = 0; i < size(); i++) {
            result &= this->v[i] == arg.v[i];
        }

        return result;
    }

    template <typename T, typename R> const T SIMD<T, R>::dot_prodoct(SIMD<T, R> const& arg) const {
        T result = 0;

        for (uint32_t i = 0; i < size(); i++) {
            result += this->v[i] * arg.v[i];
        }
        
        return result;
    }

    template <typename T, typename R> const SIMD<T, R> SIMD<T, R>::test_bit(SIMD<T, R> const& arg) const {
        SIMD<T, R> result;

        for (uint32_t i = 0; i < size(); i++) {
            result.v[i] = this->v[i] & arg.v[i] ? 1 : 0;
        }
        
        return result;
    }

    template <typename T, typename R> const SIMD<T, R> SIMD<T, R>::load(T const& arg) const {
        SIMD<T, R> result;

        for (uint32_t i = 0; i < size(); i++) {
            result.v[i] = arg;
        }
        
        return result;
    }

    template <typename T, typename R> T SIMD<T, R>::get(uint8_t index) {
        return v[index % size()];
    }

    template <typename T, typename R> void SIMD<T, R>::set(T val, uint8_t index) {
        v[index % size()] = val;
    }

    template class SIMD<uint8_t, SIMD_128>;
    template class SIMD<uint8_t, SIMD_32>;
    template class SIMD<uint8_t, SIMD_HUB75_Byte>;
    template class SIMD<uint8_t, SIMD_HUB75_Short>;
}