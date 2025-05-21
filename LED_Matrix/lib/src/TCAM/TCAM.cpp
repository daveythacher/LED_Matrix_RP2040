/* 
 * File:   tcam.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "TCAM/TCAM.h"
#include "SIMD/SIMD.h"

namespace TCAM {
    template <typename T> Table<T>::Table() {}

    template <typename T> Table<T>::Table(uint8_t size) {
        num_rules = size;
        callbacks = new Handler *[num_rules];
        masks = new T[num_rules];
        values = new T[num_rules];
	nums = new uint16_t[num_rules];

        for (uint8_t i = 0; i < num_rules; i++) {
            nums[i] = 256;
        }
    }

    template <typename T> Table<T>::~Table() {
        delete[] nums;
        delete[] masks;
        delete[] values;
    }

    template <typename T> bool Table<T>::TCAM_search(const T *data, const T *key, const T *enable) {
        return (*data & *enable) == (*key & *enable);
    }

    template <typename T> bool Table<T>::TCAM_rule(uint8_t priority, T key, T enable, uint8_t num) {
        if ((priority >= num_rules) || (callbacks[priority] != nullptr) || (callback == nullptr))
            return false;
        
        masks[priority] = key;
        values[priority] = enable;
        nums[priority] = num;

        return true;
    }

    template <typename T> bool Table<T>::TCAM_process(const T *data) {
        for (uint8_t i = 0; i < num_rules; i++) {
            if (callbacks[i] != nullptr && TCAM_search(data, &masks[i], &values[i])) {
                callbacks[i]->callback();
                return true;
            }
        }

        return false;
    }

    template class Table<SIMD::SIMD<uint8_t, SIMD::SIMD_128>>;
}
