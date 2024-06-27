#include "pico/multicore.h"
#include "Serial/serial_uart/tcam.h"

namespace Serial::TCAM {
    struct TCAM_record {
        TCAM_record() {
            claim = false;
        }

        TCAM_entry key;
        TCAM_entry enable;
        void (*func)();
        bool claim;
    };

    // Only high priority (0) rule wins
    static TCAM_record TCAM_table[num_rules];

    static bool __not_in_flash_func(TCAM_search)(TCAM_entry data, TCAM_entry key, TCAM_entry enable) {
        bool result = true;

        // Do not waste time with break
        for (uint8_t i = 0; i < sizeof(TCAM_entry) / sizeof(TCAM_entry::data); i++) {
            data.data[i] &= enable.data[i];
            key.data[i] &= enable.data[i];
            result &= data.data[i] == key.data[i];
        }

        return result;
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

    void __not_in_flash_func(TCAM_process)(TCAM_entry data) {
        bool results[sizeof(TCAM_table) / sizeof(TCAM_record)];

        for (uint8_t i = 0; i < sizeof(TCAM_table) / sizeof(TCAM_record); i++)
            results[i] = TCAM_search(data, TCAM_table[i].key, TCAM_table[i].enable);

        // TODO: We need compiler and CPU barrier

        for (uint8_t i = 0; i < sizeof(TCAM_table) / sizeof(TCAM_record); i++) {
            if (results[i]) {
                TCAM_table[i].func();
                break;
            }
        }
    }
}