/* 
 * File:   Shifter.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include <math.h>
#include "Multiplex/Multiplex.h"
#include "Multiplex/config.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"

namespace Multiplex {
    static int aux[3];
    static constexpr int ADDR_A = 16;
    static constexpr int ADDR_B = 17;
    static constexpr int ADDR_C = 18;
    static constexpr int ADDR_D = 19;
    static constexpr int ADDR_E = 20;

    void init(int rows) {
        int clk, data, lat;
        bool isThreePin = true;

        for (int i = 0; i < 5; i++) {
            gpio_init(i + 16);
            gpio_set_dir(i + 16, GPIO_OUT);
        }
        gpio_clr_mask(0x1F0000);

        switch (multiplex_type) {
            case 1:
                clk = ADDR_A;
                data = ADDR_B;
                lat = ADDR_C;
                break;
            case 0:
                // Do not use!
            default:
                break;
        }

        if (isThreePin) {
            // TODO:
            //  Add PIO shifter module

            gpio_set_function(clk, GPIO_FUNC_PIO0);
            gpio_set_function(data, GPIO_FUNC_PIO0);
            gpio_set_function(lat, GPIO_FUNC_PIO0);

            // PIO
            const uint16_t instructions[] = {
                (uint16_t) (pio_encode_pull(false, true) | pio_encode_sideset(2, 0)),   // PIO SM
                (uint16_t) (pio_encode_out(pio_x, 8) | pio_encode_sideset(2, 0)),
                (uint16_t) (pio_encode_out(pio_y, 8) | pio_encode_sideset(2, 0)),
                (uint16_t) (pio_encode_out(pio_pins, 6) | pio_encode_sideset(2, 0)),    // PMP Program
                (uint16_t) (pio_encode_jmp_y_dec(3) | pio_encode_sideset(2, 1)),
                (uint16_t) (pio_encode_nop() | pio_encode_sideset(2, 2)),
                (uint16_t) (pio_encode_nop() | pio_encode_sideset(2, 2)),
                (uint16_t) (pio_encode_nop() | pio_encode_sideset(2, 0)),
                (uint16_t) (pio_encode_jmp_x_dec(2) | pio_encode_sideset(2, 0)),
                (uint16_t) (pio_encode_jmp(0) | pio_encode_sideset(2, 0))
            };
            static const struct pio_program pio_programs = {
                .instructions = instructions,
                .length = count_of(instructions),
                .origin = 0,
            };
            pio_add_program(pio0, &pio_programs);
            pio_sm_set_consecutive_pindirs(pio0, 0, 8, 8, true);
        
            // Verify Serial Clock
            constexpr float x = 125000000.0 / (multiplex_clock * 2.0);
            static_assert(x >= 1.0, "Unabled to configure PIO for MULTIPLEX_CLOCK");

            // PMP / SM
            pio0->sm[0].clkdiv = ((uint32_t) floor(x) << 16) | ((uint32_t) round((x - floor(x)) * 255.0) << 8);
            pio0->sm[0].pinctrl = (5 << PIO_SM0_PINCTRL_SIDESET_COUNT_LSB) | (16 << PIO_SM0_PINCTRL_SIDESET_BASE_LSB);
            pio0->sm[0].shiftctrl = (1 << PIO_SM0_SHIFTCTRL_AUTOPULL_LSB) | (6 << 25) | (1 << 19);
            pio0->sm[0].execctrl = (1 << 17) | (12 << 12);
            pio0->sm[0].instr = pio_encode_jmp(0);
            hw_set_bits(&pio0->ctrl, 1 << PIO_CTRL_SM_ENABLE_LSB);
            pio_sm_claim(pio0, 0);
        }
        else {
            // TODO:
            //  Add PIO shifter module

            gpio_set_function(clk, GPIO_FUNC_PIO0);
            gpio_set_function(data, GPIO_FUNC_PIO0);

            // PIO
            const uint16_t instructions[] = {
                (uint16_t) (pio_encode_pull(false, true) | pio_encode_sideset(2, 0)),   // PIO SM
                (uint16_t) (pio_encode_out(pio_x, 8) | pio_encode_sideset(2, 0)),
                (uint16_t) (pio_encode_out(pio_y, 8) | pio_encode_sideset(2, 0)),
                (uint16_t) (pio_encode_out(pio_pins, 6) | pio_encode_sideset(2, 0)),    // PMP Program
                (uint16_t) (pio_encode_jmp_y_dec(3) | pio_encode_sideset(2, 1)),
                (uint16_t) (pio_encode_nop() | pio_encode_sideset(2, 2)),
                (uint16_t) (pio_encode_nop() | pio_encode_sideset(2, 2)),
                (uint16_t) (pio_encode_nop() | pio_encode_sideset(2, 0)),
                (uint16_t) (pio_encode_jmp_x_dec(2) | pio_encode_sideset(2, 0)),
                (uint16_t) (pio_encode_jmp(0) | pio_encode_sideset(2, 0))
            };
            static const struct pio_program pio_programs = {
                .instructions = instructions,
                .length = count_of(instructions),
                .origin = 0,
            };
            pio_add_program(pio0, &pio_programs);
            pio_sm_set_consecutive_pindirs(pio0, 0, 8, 8, true);
        
            // Verify Serial Clock
            constexpr float x = 125000000.0 / (SERIAL_CLOCK * 2.0);
            static_assert(x >= 1.0, "Unabled to configure PIO for SERIAL_CLOCK");

            // PMP / SM
            pio0->sm[0].clkdiv = ((uint32_t) floor(x) << 16) | ((uint32_t) round((x - floor(x)) * 255.0) << 8);
            pio0->sm[0].pinctrl = (2 << PIO_SM0_PINCTRL_SIDESET_COUNT_LSB) | (6 << PIO_SM0_PINCTRL_OUT_COUNT_LSB) | (14 << PIO_SM0_PINCTRL_SIDESET_BASE_LSB) | 8;
            pio0->sm[0].shiftctrl = (1 << PIO_SM0_SHIFTCTRL_AUTOPULL_LSB) | (6 << 25) | (1 << 19);
            pio0->sm[0].execctrl = (1 << 17) | (12 << 12);
            pio0->sm[0].instr = pio_encode_jmp(0);
            hw_set_bits(&pio0->ctrl, 1 << PIO_CTRL_SM_ENABLE_LSB);
            pio_sm_claim(pio0, 0);
        }
    }
    
    void __not_in_flash_func(SetRow)(int row) {
        // TODO:

        switch (multiplex_type) {

            case 0:
                // Do not use!
            default:
                break;
        }
    }
}
