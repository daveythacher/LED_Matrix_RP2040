/* 
 * File:   tcam.h
 * Author: David Thacher
 * License: GPL 3.0
 */

#ifndef TCAM_H
#define TCAM_H

#include <stdint.h>
#include "SIMD/SIMD.h"

// This can be rendered into coprocessor.
namespace Serial::TCAM {
    // Future: Add banks
    constexpr uint8_t num_rules = 4;
    constexpr uint8_t num_longs = 3;

    union TCAM_entry {
        uint8_t bytes[num_longs * 4];
        uint16_t shorts[num_longs * 2];
        uint32_t data[num_longs];
        SIMD::SIMD_QUARTER<uint32_t> vec[num_longs];
    };

    // Only the highest priority rule match runs

    bool TCAM_rule(uint8_t priority, TCAM_entry key, TCAM_entry enable, void (*func)());
    void TCAM_process(const TCAM_entry *data);
}

#endif