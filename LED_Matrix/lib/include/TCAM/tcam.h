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
namespace TCAM {
    // Future: Add banks
    constexpr uint8_t num_rules = 4;
    constexpr uint8_t num_longs = sizeof(SIMD::SIMD_SINGLE<uint32_t>) / sizeof(uint32_t);

    union TCAM_entry {
        uint8_t bytes[num_longs * 4];
        uint16_t shorts[num_longs * 2];
        uint32_t data[num_longs];
        SIMD::SIMD_SINGLE<uint32_t> vec;
    };

    // Only the highest priority rule match runs

    bool TCAM_rule(uint8_t priority, TCAM_entry key, TCAM_entry enable, void (*func)());
    void TCAM_process(const TCAM_entry *data);
}

#endif