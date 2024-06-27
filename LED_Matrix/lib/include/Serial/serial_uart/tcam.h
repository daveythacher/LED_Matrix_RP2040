/* 
 * File:   tcam.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#include <stdint.h>

namespace Serial::TCAM {
    constexpr uint8_t num_rules = 4;

    struct TCAM_entry {
        uint32_t data[3];
    };

    // Only the highest priority rule match runs

    bool TCAM_rule(uint8_t priority, TCAM_entry key, TCAM_entry enable, void (*func)());
    void TCAM_process(TCAM_entry data);
}