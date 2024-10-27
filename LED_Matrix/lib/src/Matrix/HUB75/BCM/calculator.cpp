/* 
 * File:   calculator.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include <algorithm>
#include <stdint.h>
#include "Matrix/HUB75/BCM/memory_format.h"
#include "Matrix/matrix.h"
#include "Serial/config.h"

namespace Matrix::Calculator {
    // Panel constants
    constexpr double max_clk_mhz = 25.0;
    constexpr uint8_t columns_per_driver = 16;
    constexpr uint8_t fanout_per_clk = 6;
    constexpr uint8_t max_impedance = 50;               // For longer chains may need custom circuit board which adjusts this
    constexpr uint8_t max_par_cap_pf = 18;              // For longer chains may need custom circuit board which adjusts this
    constexpr uint8_t min_harmonics = 5;

    // LED constants
    constexpr uint16_t max_led_impedance = 1650;
    constexpr uint8_t max_led_cap_pf = 18;
    constexpr uint8_t min_led_harmonics = 5;

    static constexpr double get_refresh_overhead() {
        double refresh_overhead = 1000000.0 / (MULTIPLEX * MIN_REFRESH);
        refresh_overhead /= refresh_overhead - BLANK_TIME;
        return refresh_overhead;
    }

    static constexpr void is_clk_valid() {
        constexpr uint64_t temp = (COLUMNS / columns_per_driver) * (max_impedance * fanout_per_clk * min_harmonics * max_par_cap_pf);
        constexpr double hz_limit = BYPASS_FANOUT ? max_clk_mhz * 1000000.0 : 
            std::min(max_clk_mhz, (double) (1000000.0 / (temp * 1.0))) * 1000000.0;
        constexpr double clk_hz = hz_limit / (MIN_REFRESH * get_refresh_overhead() * COLUMNS * MULTIPLEX * (1 << PWM_bits));

        static_assert(SERIAL_CLOCK <= hz_limit, "Serial clock is too high");
        static_assert(clk_hz >= 1.0, "Configuration is not possible");
    }

    static constexpr void is_brightness_valid() {
        constexpr double led_rise_us = (max_led_impedance * min_led_harmonics * max_led_cap_pf * MULTIPLEX) / 1000000.0;
        constexpr double period_us = 1000000.0 / (MIN_REFRESH * MULTIPLEX);
        constexpr double brightness = ((period_us / get_refresh_overhead()) - (led_rise_us * PWM_bits / 2.0)) / period_us;
        constexpr double accuracy = ((period_us / get_refresh_overhead()) - (led_rise_us * PWM_bits / 2.0)) / (period_us / get_refresh_overhead());

        static_assert(brightness > 0.75, "Brightness less than 75 percent is not recommended");
        static_assert(accuracy > 0.95, "Accuracy less than 95 percent is not recommended");
    }

    static constexpr void is_blank_time_valid() {
        constexpr double led_fall_us_high = (10000 * COLUMNS * max_led_cap_pf) / 1000000.0;

        static_assert(led_fall_us_high < BLANK_TIME, "Blank time is too low for high side");
    }

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
    void verify_configuration() {
        is_brightness_valid();
        is_clk_valid();
        is_blank_time_valid();
        
        static_assert(COLUMNS >= columns_per_driver, "COLUMNS less than 8 is not recommended");
        static_assert(COLUMNS <= 256, "COLUMNS more than 1024 is not recommended, but we only support up to 256");
        static_assert((2 * MULTIPLEX * COLUMNS) <= 8192, "More than 8192 pixels is not recommended");
        static_assert((2 * MULTIPLEX * COLUMNS * sizeof(Serial::DEFINE_SERIAL_RGB_TYPE)) <= Serial::payload_size, "The current frame size is not supported");
        static_assert((MULTIPLEX * COLUMNS * (PWM_bits)) <= Serial::max_framebuffer_size, "The current buffer size is not supported");
        static_assert((MULTIPLEX * (1 << PWM_bits)) <= (4 * 1024), "The current LED grayscale is not supported");
        static_assert(MIN_REFRESH > 2 * FPS, "Refresh rate must be higher than twice the number of frames per second");

        // Qualify Worker Performance
        static_assert(((2 * MULTIPLEX * COLUMNS * 3 * 2 * FPS * PWM_bits) / 1000000.0) <= 1.5, "CPU is only capable of so many operations per second.");
    }
}