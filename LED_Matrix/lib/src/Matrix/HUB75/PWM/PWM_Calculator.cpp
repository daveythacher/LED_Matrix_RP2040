/* 
 * File:   PWM_Calculator.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include <algorithm>
#include "Matrix/HUB75/PWM/PWM_Calculator.h"

// TODO: Sort this out

namespace Matrix {
    static bool result;

    /* TODO:

    // Panel constants
    constexpr double max_clk_mhz = 25.0;
    constexpr uint8_t columns_per_driver = 16;
    constexpr uint8_t fanout_per_clk = 6;
    constexpr uint8_t max_impedance = 50;               // For longer chains may need custom circuit board which adjusts this
    constexpr uint8_t max_par_cap_pf = 18;              // For longer chains may need custom circuit board which adjusts this
    constexpr uint8_t min_harmonics = 10;               // This should be at least 10 for square waves

    // LED constants
    constexpr uint16_t max_led_impedance = 1650;
    constexpr uint8_t max_led_cap_pf = 18;
    constexpr uint8_t min_led_harmonics = 5;

    static void custom_assert(bool condition, const char *message) {
        result &= condition;
    }

    static constexpr double get_refresh_overhead() {
        double refresh_overhead = 1000000.0 / (MULTIPLEX * MIN_REFRESH);
        refresh_overhead /= refresh_overhead - BLANK_TIME;
        return refresh_overhead;
    }

    static constexpr void is_clk_valid() {
        constexpr uint64_t temp = (COLUMNS / columns_per_driver) * max_impedance * fanout_per_clk * min_harmonics * max_par_cap_pf;
        constexpr double hz_limit = BYPASS_FANOUT ? max_clk_mhz * 1000000.0 : 
            std::min(max_clk_mhz, (double) (1000000.0 / (temp * 1.0))) * 1000000.0;
        constexpr double clk_hz = hz_limit / (MIN_REFRESH * get_refresh_overhead() * COLUMNS * MULTIPLEX * ((1 << PWM_bits) + 1));
        constexpr float x = 125000000.0 / (SERIAL_CLOCK * 2.0);     // Someday this two will be a four.
        
        custom_assert(SERIAL_CLOCK <= hz_limit, "Serial clock is too high");
        custom_assert(x >= 1.0, "Unabled to configure PIO for SERIAL_CLOCK");
        custom_assert(clk_hz >= 1.0, "Configuration is not possible");
    }

    static constexpr void is_brightness_valid() {
        constexpr double led_rise_us = (max_led_impedance * min_led_harmonics * max_led_cap_pf * MULTIPLEX) / 1000000.0;
        constexpr double period_us = 1000000.0 / (MIN_REFRESH * MULTIPLEX);
        constexpr double brightness = ((period_us / get_refresh_overhead()) - led_rise_us) / period_us;
        constexpr double accuracy = ((period_us / get_refresh_overhead()) - led_rise_us) / (period_us / get_refresh_overhead());

        custom_assert(brightness > 0.75, "Brightness less than 75 percent is not recommended");
        custom_assert(accuracy > 0.95, "Accuracy less than 95 percent is not recommended");
    }

    static constexpr void is_blank_time_valid() {
        constexpr double led_fall_us_high = (10000 * COLUMNS * max_led_cap_pf) / 1000000.0;

        custom_assert(led_fall_us_high < BLANK_TIME, "Blank time is too low for high side");
    }*/

    /*  Must test the following:
     *      1) Fanout
     *      2) Refresh
     *      3) Memory usage
     *      4) Computation operations required
     *  Additional Tests:
     *      1) Grayscale limits
     *      2) Ghosting limits
     *      3) Accuracy of signals
     */
    bool verify_configuration() {
        result = true;

        /* TODO:
        is_brightness_valid();
        is_clk_valid();
        is_blank_time_valid();

        // This could be 8 or 16 depending on panel. (Kind of random.)
        custom_assert(COLUMNS >= columns_per_driver, "COLUMNS less than 8 is not recommended");

        // This is depends on panel implementation however the fanout and par cap in matrix limit the max size.
        //  Technically capable of more pixels with multiplexing, if we reduce refresh and contrast.
        //  Picked numbers to simplify support and define limits.
        custom_assert(COLUMNS <= 255, "COLUMNS more than 1024 is not recommended, but we only support up to 255");
        custom_assert((2 * MULTIPLEX * COLUMNS) <= 8192, "More than 8192 pixels is not recommended");

        // This is the limit observed in testing and from most panel specifications.
        custom_assert((MULTIPLEX * (1 << PWM_bits)) <= (4 * 1024), "The current LED grayscale is not supported");

        // There is a trade off here: (Is most extreme for this matrix algorithm.)
        //  1. Increase the frame size and do remote computation. (This works up to 96KB, which requires at least 25Mbps in serial algorithm.)
        //      Currently this is not used and set to 12KB, which requires at least 7.8Mbps in serial algorithm. (Local CPU is used to compute bitplanes.)
        //          Note 7.8Mbps allows for main processor jitter and 30 frames per second. (7.8Mbps reduces termination complexity.)
        //  2. Use local computation which constrains the buffer size. (This works up to 48KB, which requires all of core 1.)
        //      Currently this is used and is constrained to 48KB, which requires all of core 1.
        //
        //  The sum off all memory usage for serial frames and LED buffers must not exceed 192KB.
        //      64KB is reserved for code and 8KB is reserved for stack/heap for both cores.
        custom_assert((2 * MULTIPLEX * COLUMNS * sizeof(Serial::DEFINE_SERIAL_RGB_TYPE)) <= Serial::payload_size, "The current frame size is not supported");
        custom_assert((MULTIPLEX * COLUMNS * (1 << PWM_bits)) <= Serial::max_framebuffer_size, "The current buffer size is not supported");
        custom_assert(MIN_REFRESH > 2 * FPS, "Refresh rate must be higher than twice the number of frames per second");
        custom_assert(Serial::num_framebuffers > 2, "Must have at least three framebuffers");

        // Qualify Worker Performance
        custom_assert(((2 * MULTIPLEX * COLUMNS * 3 * 2 * FPS * (1 << PWM_bits)) / 1000000.0) <= 1.5, "CPU is only capable of so many operations per second.");*/

        return result;
    }
}
