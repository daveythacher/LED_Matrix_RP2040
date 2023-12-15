#include <algorithm>
#include <stdint.h>
#include "Matrix/PWM/memory_format.h"
#include "Matrix/matrix.h"

namespace Matrix::Calculator {
    // Panel constants
    constexpr float max_clk_mhz = 25.0;
    constexpr uint8_t columns_per_driver = 16;
    constexpr uint8_t fanout_per_clk = 6;
    constexpr uint8_t max_impedance = 50;               // For longer chains may need custom circuit board which adjusts this
    constexpr uint8_t max_par_cap_pf = 16;              // For longer chains may need custom circuit board which adjusts this
    constexpr uint8_t min_harmonics = 5;
    constexpr uint16_t max_refresh_hz = 3000;


    // LED constants
    constexpr uint16_t max_led_impedance = 500;
    constexpr uint8_t max_led_cap_pf = 15;
    constexpr uint8_t min_led_harmonics = 5;

    static constexpr float get_refresh_overhead() {
        float refresh_overhead = 1000000.0 / (MULTIPLEX * MAX_REFRESH);
        refresh_overhead /= refresh_overhead - BLANK_TIME;
        return refresh_overhead;
    }

    static constexpr void is_clk_valid() {
        constexpr uint64_t temp = (COLUMNS / columns_per_driver) * max_impedance * fanout_per_clk * min_harmonics * max_par_cap_pf;
        constexpr float hz_limit = BYPASS_FANOUT ? max_clk_mhz * 1000000.0 : 
            std::min(max_clk_mhz, (float) (1000000.0 / (temp * 1.0))) * 1000000.0;
        constexpr float clk_hz = hz_limit / (MAX_REFRESH * get_refresh_overhead() * COLUMNS * MULTIPLEX * (1 << PWM_bits));
        
        static_assert(SERIAL_CLOCK <= hz_limit, "Serial clock is too high");
        static_assert(clk_hz >= 1.0, "Configuration is not possible");
    }

    static constexpr void is_brightness_valid() {
        constexpr float led_rise_us = (max_led_impedance * min_led_harmonics * max_led_cap_pf * MULTIPLEX) / 1000000.0;
        constexpr float period_us = 1000000.0 / (MAX_REFRESH * MULTIPLEX);
        constexpr float brightness = ((period_us / get_refresh_overhead()) - led_rise_us) / period_us;
        constexpr float accuracy = ((period_us / get_refresh_overhead()) - (led_rise_us * PWM_bits)) / (period_us / get_refresh_overhead());

        static_assert(brightness > 0.75, "Brightness less than 75 percent is not recommended");
        static_assert(accuracy > 0.95, "Accuracy less than 95 percent is not recommended");
    }

    static constexpr void is_blank_time_valid() {
        constexpr float led_rise_us_low = (max_led_impedance * COLUMNS * min_led_harmonics * max_led_cap_pf) / 1000000.0;
        constexpr float led_fall_us_high = (1000 * COLUMNS * min_led_harmonics * max_led_cap_pf) / 1000000.0;

        static_assert(led_rise_us_low < BLANK_TIME, "Blank time is too low for low side");
        static_assert(led_fall_us_high < BLANK_TIME, "Blank time is too low for high side");
    }

    void verify_configuration() {
        is_brightness_valid();
        is_clk_valid();
        is_blank_time_valid();

        static_assert(MAX_REFRESH > 2 * FPS, "Refresh rate must be higher than twice the number of frames per second");
    }
}
