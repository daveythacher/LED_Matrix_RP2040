#include <stdint.h>
#include <cmath>
#include "SIMD/SIMD.h"

namespace SIMD {
    template <typename T> SIMD_QUARTER<T> SIMD_QUARTER<T>::operator|(SIMD_QUARTER<T> const& arg) {
        SIMD_QUARTER<T> result;
        result.l = this->l | arg.l;
        return result;
    }

    template class SIMD_QUARTER<uint8_t>;
}