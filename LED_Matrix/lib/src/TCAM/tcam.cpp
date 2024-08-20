#include "pico/multicore.h"
#include "TCAM/tcam.h"

namespace TCAM {
    struct TCAM_record {
        TCAM_record() {
            claim = false;
            key = { 0 };
            enable = { 0 };
        }

        TCAM_entry key;
        TCAM_entry enable;
        void (*func)();
        bool claim;
    };

    // Only high priority (0) rule wins
    static TCAM_record TCAM_table[num_rules];

    static bool __not_in_flash_func(TCAM_search)(const TCAM_entry *data, const TCAM_entry *key, const TCAM_entry *enable) {
        SIMD::SIMD_SINGLE<uint32_t> d = data->vec & enable->vec;
        SIMD::SIMD_SINGLE<uint32_t> k = key->vec & enable->vec;
        return d == k;
    }

    bool TCAM_rule(uint8_t priority, TCAM_entry key, TCAM_entry enable, void (*func)()) {
        if (priority >= num_rules)
            return false;

        if (TCAM_table[priority].claim)
            return false;
        
        TCAM_table[priority].key = key;
        TCAM_table[priority].enable = enable;
        TCAM_table[priority].func = func;
        TCAM_table[priority].claim = true;
        return true;
    }

    void __not_in_flash_func(TCAM_process)(const TCAM_entry *data) {
        bool results[sizeof(TCAM_table) / sizeof(TCAM_record)];

        for (uint8_t i = 0; i < sizeof(TCAM_table) / sizeof(TCAM_record); i++)
            results[i] = TCAM_search(data, &TCAM_table[i].key, &TCAM_table[i].enable);

        // Future: We need compiler and CPU barrier

        for (uint8_t i = 0; i < sizeof(TCAM_table) / sizeof(TCAM_record); i++) {
            if (results[i]) {
                TCAM_table[i].func();
                break;
            }
        }
    }
}