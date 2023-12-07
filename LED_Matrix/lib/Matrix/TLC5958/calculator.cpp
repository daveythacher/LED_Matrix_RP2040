#include <algorithm>

// Panel constants
constexpr float max_clk_mhz = 25.0;
constexpr float max_gclk_mhz = 33.0;
constexpr uint8_t columns_per_driver = 16;
constexpr uint8_t fanout_per_clk = 6;
constexpr uint8_t fanout_per_gclk = 1;
constexpr uint8_t max_impedance = 50;               // For longer chains may need custom circuit board which adjusts this
constexpr uint8_t max_par_cap_pf = 16;              // For longer chains may need custom circuit board which adjusts this
constexpr uint8_t min_harmonics = 5;
constexpr uint16_t max_refresh_hz = 3000;


// LED constants
constexpr uint16_t max_led_impedance = 500;
constexpr uint8_t max_led_cap_pf = 15;
constexpr uint8_t min_led_harmonics = 5;

float get_refresh_overhead(uint8_t scan, uint16_t refresh, uint8_t bits) {
    float refresh_overhead = 1000000.0 / (scan * refresh);
    if (isS_PWM)
        refresh_overhead /= 1 << std::max((int16_t) bits - (int16_t) s_pwm_bits_per_seg + (int16_t) get_min_dot_correction_bits(), 0);
    refresh_overhead /= refresh_overhead - blank_time_us;
    return refresh_overhead;
}

static bool is_clk_valid(uint8_t scan, uint16_t cols, uint16_t refresh, uint8_t bits, float *clk_mhz) {
    uint64_t temp = cols / columns_per_driver;
    temp *= max_impedance * fanout_per_clk * min_harmonics * max_par_cap_pf;
    float hz_limit = 1000000.0 / (temp * 1.0);
    hz_limit = std::min(max_clk_mhz, hz_limit) * 1000000;
    *clk_mhz = hz_limit / 1000000.0;
    hz_limit /= frames_per_second * cols * scan * 16;
    *clk_mhz /= hz_limit;
    return hz_limit >= 1.0;
}

static bool is_gclk_valid(uint8_t scan, uint16_t cols, uint16_t refresh, uint8_t bits, float *gclk_mhz) {
    uint64_t temp = cols / columns_per_driver;
    temp *= max_impedance * fanout_per_gclk * min_harmonics * max_par_cap_pf;
    float hz_limit = 1000000.0 / (temp * 1.0);
    hz_limit = std::min(max_gclk_mhz, hz_limit) * 1000000;
    *gclk_mhz = hz_limit / 1000000.0;
    hz_limit /= refresh * get_refresh_overhead(scan, refresh, bits) * scan * (1 << (bits + get_min_dot_correction_bits()));
    *gclk_mhz /= hz_limit;
    return hz_limit >= 1.0;
}

static bool is_grayscale_valid(uint8_t scan, uint16_t cols, uint16_t refresh, uint8_t bits) {
    return ((1 << max_grayscale_bits) / ((1 << (bits + get_min_dot_correction_bits())) * scan)) >= 1;
}

static bool is_gen_3_valid(uint8_t scan, uint16_t cols, uint16_t refresh, uint8_t bits, float *clk_mhz, float *gclk_mhz, float *brightness) {
    float led_rise_us = (max_led_impedance * min_led_harmonics * max_led_cap_pf * scan) / 1000000.0;
    float period_us = 1000000.0 / (refresh * scan);
    *brightness = ((period_us / get_refresh_overhead(scan, refresh, bits)) - led_rise_us) / period_us;

    return (is_clk_valid(scan, cols, refresh, bits, clk_mhz) &&
        is_gclk_valid(scan, cols, refresh, bits, gclk_mhz) &&
        *brightness > target_brightness &&
        is_grayscale_valid(scan, cols, refresh, bits));
}

void process_gen3() {
    // Order of bottlneck: pixels, refresh, multiplexing, grayscale.
    for (uint16_t cols = cols_low; cols <= cols_high; cols *= 2) {
        for (uint16_t refresh = refresh_low; refresh <= refresh_high; refresh += 100) {
            for (uint8_t scan = scan_low; scan <= scan_high; scan *= 2) {
                for (uint8_t bits = min_bpp_bits; bits <= max_grayscale_bits; bits++) {
                    float clk, gclk, brightness;

                    if (((scan * 2) != cols) && ((scan * 4)) != cols && !showAll)
                        continue;
                    else if (is_gen_3_valid(scan, cols, refresh, bits, &clk, &gclk, &brightness))
                        print_result(scan, cols, refresh, bits, clk, gclk, brightness);
                    else 
                        break;
                }
            }
        }
    }
}