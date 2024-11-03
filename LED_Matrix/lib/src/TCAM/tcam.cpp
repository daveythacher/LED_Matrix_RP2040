#include "pico/multicore.h"
#include "TCAM/tcam.h"

namespace TCAM {
    template <typename T> Table<T>::Table() {
        for (uint8_t i = 0; i < num_rules; i++) {
            callbacks[i] = nullptr;
        }
    }

    template <typename T> bool __not_in_flash_func(Table<T>::TCAM_search)(const T *data, const T *key, const T *enable) {
        return (*data & *enable) == (*key & *enable);
    }

    template <typename T> bool Table<T>::TCAM_rule(uint8_t priority, T key, T enable, Handler *callback) {
        if ((priority >= num_rules) || (callbacks[priority] != nullptr) || (callback == nullptr))
            return false;
        
        masks[priority] = key;
        values[priority] = enable;
        callbacks[priority] = callback;

        return true;
    }

    template <typename T> void __not_in_flash_func(Table<T>::TCAM_process)(const T *data) {
        for (uint8_t i = 0; i < num_rules; i++) {
            if (TCAM_search(data, &masks[i], &values[i])) {
                callbacks[i]->callback();
                break;
            }
        }
    }

    template class Table<SIMD::SIMD_SINGLE<uint32_t>>;
}