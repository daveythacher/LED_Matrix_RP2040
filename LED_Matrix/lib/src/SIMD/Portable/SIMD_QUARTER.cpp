#include "SIMD/SIMD_QUARTER.h"

namespace SIMD {
    template <typename T> const SIMD_QUARTER<T> SIMD_QUARTER<T>::operator|(SIMD_QUARTER<T> const& arg) const {
        SIMD_QUARTER<T> result;
        result.l = this->l | arg.l;
        return result;
    }

    template <typename T> const SIMD_QUARTER<T> SIMD_QUARTER<T>::operator&(SIMD_QUARTER<T> const& arg) const {
        SIMD_QUARTER<T> result;
        result.l = this->l & arg.l;
        return result;
    }

    template <typename T> const bool SIMD_QUARTER<T>::operator==(SIMD_QUARTER<T> const& arg) const {
        bool result = true;
        
        for (uint32_t i = 0; i < SIMD_QUARTER<T>::size(); i++) {
            result &= this->v[i] == arg.v[i];
        }

        return result;
    }

    template class SIMD_QUARTER<uint8_t>;
}