#include <algorithm>
#include <stdint.h>
#include "Matrix/PWM/memory_format.h"
#include "Matrix/matrix.h"

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
        constexpr uint64_t temp = (COLUMNS / columns_per_driver) * max_impedance * fanout_per_clk * min_harmonics * max_par_cap_pf;
        constexpr double hz_limit = BYPASS_FANOUT ? max_clk_mhz * 1000000.0 : 
            std::min(max_clk_mhz, (double) (1000000.0 / (temp * 1.0))) * 1000000.0;
        constexpr double clk_hz = hz_limit / (MIN_REFRESH * get_refresh_overhead() * COLUMNS * MULTIPLEX * (1 << PWM_bits));
        
        static_assert(SERIAL_CLOCK <= hz_limit, "Serial clock is too high");
        static_assert(clk_hz >= 1.0, "Configuration is not possible");
    }

    static constexpr void is_brightness_valid() {
        constexpr double led_rise_us = (max_led_impedance * min_led_harmonics * max_led_cap_pf * MULTIPLEX) / 1000000.0;
        constexpr double period_us = 1000000.0 / (MIN_REFRESH * MULTIPLEX);
        constexpr double brightness = ((period_us / get_refresh_overhead()) - led_rise_us) / period_us;
        constexpr double accuracy = ((period_us / get_refresh_overhead()) - led_rise_us) / (period_us / get_refresh_overhead());

        static_assert(brightness > 0.75, "Brightness less than 75 percent is not recommended");
        static_assert(accuracy > 0.95, "Accuracy less than 95 percent is not recommended");
    }

    static constexpr void is_blank_time_valid() {
        constexpr double led_fall_us_high = (10000 * COLUMNS * max_led_cap_pf) / 1000000.0;

        static_assert(led_fall_us_high < BLANK_TIME, "Blank time is too low for high side");
    }

    void verify_configuration() {
        is_brightness_valid();
        is_clk_valid();
        is_blank_time_valid();

        static_assert(MIN_REFRESH > 2 * FPS, "Refresh rate must be higher than twice the number of frames per second");
    }
}
