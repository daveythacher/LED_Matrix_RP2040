#include <stdint.h>

namespace Serial::TCAM {
    struct TCAM_entry {
        uint32_t data[3];
    };

    struct TCAM_record {
        TCAM_entry key;
        TCAM_entry enable;
        void (*func)();
    };

    // High priority (0) wins
    //  I do not support multiple handlers per search by design
    TCAM_record TCAM_table[4];

    // We can speculate this?
    //  This is pure as long as cache is clean?
    bool TCAM_search(TCAM_entry data, TCAM_entry key, TCAM_entry enable) {
        bool result = true;

        // Do not waste time with break
        for (uint8_t i = 0; i < sizeof(TCAM_entry) / sizeof(TCAM_entry::data); i++) {
            data.data[i] &= enable.data[i];
            key.data[i] &= enable.data[i];
            result &= data.data[i] == key.data[i];
        }

        return result;
    }

    void TCAM_process(TCAM_entry data) {
        bool results[sizeof(TCAM_table) / sizeof(TCAM_record)];

        // We can unroll this into pure functions or threads?
        for (uint8_t i = 0; i < sizeof(TCAM_table) / sizeof(TCAM_record); i++)
            results[i] = TCAM_search(data, TCAM_table[i].key, TCAM_table[i].enable);

        // TODO: We need compiler and CPU barrier

        // We should not speculate this?
        for (uint8_t i = 0; i < sizeof(TCAM_table) / sizeof(TCAM_record); i++) {
            if (results[i]) {
                TCAM_table[i].func();
                break;
            }
        }
    }
}