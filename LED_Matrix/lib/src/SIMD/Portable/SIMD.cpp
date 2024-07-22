#include "SIMD/SIMD.h"

// TODO: Move these into their own file
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

    template <typename T> const SIMD_SINGLE<T> SIMD_SINGLE<T>::operator|(SIMD_SINGLE<T> const& arg) const {
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

    template <typename T> const bool SIMD_SINGLE<T>::operator==(SIMD_SINGLE<T> const& arg) const {
        bool result = true;
        
        for (uint32_t i = 0; i < SIMD_SINGLE<T>::size(); i++) {
            result &= this->v[i] == arg.v[i];
        }

        return result;
    }

    template class SIMD_QUARTER<uint8_t>;
    template class SIMD_SINGLE<uint32_t>;
}