/* 
 * File:   Shifter.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include <math.h>
#include "Multiplex/Multiplex.h"
#include "Multiplex/HUB75/hw_config.h"
#include "Matrix/config.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"

namespace Multiplex {
    static int aux[3];
    static constexpr int ADDR_A = Multiplex::HUB75::HUB75_ADDR_BASE;
    static constexpr int ADDR_B = Multiplex::HUB75::HUB75_ADDR_BASE + 1;
    static constexpr int ADDR_C = Multiplex::HUB75::HUB75_ADDR_BASE + 2;
    static constexpr int ADDR_D = Multiplex::HUB75::HUB75_ADDR_BASE + 3;
    static constexpr int ADDR_E = Multiplex::HUB75::HUB75_ADDR_BASE + 4;

    void init(int start_flag, int signal_flag) {
        int clk, data, lat;
        uint8_t type = 0;

        // TODO: Upgrade to new format

        for (int i = 0; i < Multiplex::HUB75::HUB75_ADDR_LEN; i++) {
            gpio_init(i + Multiplex::HUB75::HUB75_ADDR_BASE);
            gpio_set_dir(i + Multiplex::HUB75::HUB75_ADDR_BASE, GPIO_OUT);
        }
        gpio_clr_mask(0x1F0000);

        switch (multiplex_type) {
            case 1:
                clk = ADDR_A;
                data = ADDR_B;
                lat = ADDR_C;
                break;
            case 2:
                clk = ADDR_A;
                data = ADDR_B;
                type = 1;
                break;
            case 3:
                clk = ADDR_A;
                data = ADDR_B;
                lat = ADDR_C;
                aux[0] = ADDR_D;
                aux[1] = ADDR_E;
                break;
            case 0:
                // Do not use!
            default:
                break;
        }

        switch (type) {
            case 0: // CLK, DATA, LAT version
                {
                    // TODO:
                    //  Update PIO code
                    //      Make blocking logic to allow syncing

                    gpio_set_function(clk, GPIO_FUNC_PIO1);
                    gpio_set_function(data, GPIO_FUNC_PIO1);
                    gpio_set_function(lat, GPIO_FUNC_PIO1);

                    // while(x--) {
                    //     if (y--) {
                    //         data, clk = 0, 1
                    //     }
                    //     else {
                    //         data, clk = 1, 1
                    //         break
                    //     }
                    // }
                    // while (x--) {
                    //     data, clk = 0, 1
                    // }
                    // lat, data, clk = 1, 0, 0

                    // entry:
                    //     str pins, 0
                    //     ldb osr, fifo
                    //     str pins, 0
                    //     mov x, osr
                    //     str pins, 0
                    //     ldb osr, fifo
                    //     str pins, 0
                    //     mov y, osr
                    // loop1_begin:
                    //     cmp x, 0
                    //     subi x, x, 1
                    //     str pins, 0
                    //     bne loop1   // This should always branch
                    // loop2_begin:
                    //     cmp x, 0
                    //     subi x, x, 1
                    //     str pins, 0
                    //     bne loop2
                    //     str pins, lat
                    //     nop
                    //     str pins, 0
                    //     jmp entry
                    // loop1:
                    //     cmp y, 0
                    //     subi y, y, 1
                    //     str pins, 0
                    //     bne internal
                    //     str pins, clk, data
                    //     jmp loop2_begin
                    // internal:
                    //     str pins, clk
                    //     jmp loop1_begin
                    // loop2:
                    //     str pins, clk
                    //     jmp loop2_begin

                    // PIO
                    const uint16_t instructions[] = {
                        (uint16_t) (pio_encode_pull(false, true) | pio_encode_sideset(5, 0)),                                           // Line 0
                        (uint16_t) (pio_encode_mov(pio_x, pio_osr) | pio_encode_sideset(5, 0)),                                         // Line 1
                        (uint16_t) (pio_encode_pull(false, true) | pio_encode_sideset(5, 0)),                                           // Line 2
                        (uint16_t) (pio_encode_mov(pio_y, pio_osr) | pio_encode_sideset(5, 0)),                                         // Line 3
                        (uint16_t) (pio_encode_jmp_x_dec(8) | pio_encode_sideset(5, 0)),                                                // Line 4 (This should never advance to Line 5!!!)
                        (uint16_t) (pio_encode_jmp_x_dec(11) | pio_encode_sideset(5, 0)),                                               // Line 5
                        (uint16_t) (pio_encode_nop() | pio_encode_sideset(5, (1 << (lat - ADDR_A)))),                                   // Line 6                                               // Line 5
                        // TODO: Add sync point here
                        (uint16_t) (pio_encode_jmp(0) | pio_encode_sideset(5, 0)),                                                      // Line 7
                        (uint16_t) (pio_encode_jmp_y_dec(10) | pio_encode_sideset(5, 0)),                                               // Line 8
                        (uint16_t) (pio_encode_jmp(5) | pio_encode_sideset(5, (1 << (clk - ADDR_A)) | (1 << (data - ADDR_A)))),         // Line 9
                        (uint16_t) (pio_encode_jmp(4) | pio_encode_sideset(5, (1 << (clk - ADDR_A)))),                                  // Line 10
                        (uint16_t) (pio_encode_jmp(5) | pio_encode_sideset(5, (1 << (clk - ADDR_A)))),                                  // Line 11
                    };
                    static const struct pio_program pio_programs = {
                        .instructions = instructions,
                        .length = count_of(instructions),
                        .origin = 0,
                    };
                    pio_add_program(pio1, &pio_programs);
                    pio_sm_set_consecutive_pindirs(pio1, 0, Multiplex::HUB75::HUB75_ADDR_BASE, 5, true);
                
                    // Verify Serial Clock
                    constexpr float x = 125000000.0 / (multiplex_clock * 2.0);     // Someday this two will be a four.
                    static_assert(x >= 1.0, "Unabled to configure PIO for MULTIPLEX_CLOCK");

                    // PMP / SM
                    pio1->sm[0].clkdiv = ((uint32_t) floor(x) << PIO_SM0_CLKDIV_INT_LSB) | ((uint32_t) round((x - floor(x)) * 255.0) << PIO_SM0_CLKDIV_FRAC_LSB);
                    pio1->sm[0].pinctrl = (5 << PIO_SM0_PINCTRL_SIDESET_COUNT_LSB) | (Multiplex::HUB75::HUB75_ADDR_BASE << PIO_SM0_PINCTRL_SIDESET_BASE_LSB);
                    pio1->sm[0].shiftctrl = (1 << PIO_SM0_SHIFTCTRL_AUTOPULL_LSB) | (8 << PIO_SM0_SHIFTCTRL_PULL_THRESH_LSB);
                    pio1->sm[0].execctrl = (12 << PIO_SM0_EXECCTRL_WRAP_TOP_LSB);
                    pio1->sm[0].instr = pio_encode_jmp(0);
                    hw_set_bits(&pio1->ctrl, 1 << PIO_CTRL_SM_ENABLE_LSB);
                    pio_sm_claim(pio1, 0);
                }
                break;
            case 1: // CLK, DATA version
                {
                    // TODO:
                    //  Update PIO code
                    //      Make blocking logic to allow syncing

                    gpio_set_function(clk, GPIO_FUNC_PIO1);
                    gpio_set_function(data, GPIO_FUNC_PIO1);

                    // while(x--) {
                    //     if (y--) {
                    //         data, clk = 0, 1
                    //     }
                    //     else {
                    //         data, clk = 1, 1
                    //         break
                    //     }
                    // }
                    // while (x--) {
                    //     data, clk = 0, 1
                    // }

                    // entry:
                    //     str pins, 0
                    //     ldb osr, fifo
                    //     str pins, 0
                    //     mov x, osr
                    //     str pins, 0
                    //     ldb osr, fifo
                    //     str pins, 0
                    //     mov y, osr
                    // loop1_begin:
                    //     cmp x, 0
                    //     subi x, x, 1
                    //     str pins, 0
                    //     bne loop1   // This should always branch
                    // loop2_begin:
                    //     cmp x, 0
                    //     subi x, x, 1
                    //     str pins, 0
                    //     bne loop2
                    //     str pins, 0
                    //     jmp entry
                    // loop1:
                    //     cmp y, 0
                    //     subi y, y, 1
                    //     str pins, 0
                    //     bne internal
                    //     str pins, clk, data
                    //     jmp loop2_begin
                    // internal:
                    //     str pins, clk
                    //     jmp loop1_begin
                    // loop2:
                    //     str pins, clk
                    //     jmp loop2_begin

                    // PIO
                    const uint16_t instructions[] = {
                        (uint16_t) (pio_encode_pull(false, true) | pio_encode_sideset(5, 0)),                                           // Line 0
                        (uint16_t) (pio_encode_mov(pio_x, pio_osr) | pio_encode_sideset(5, 0)),                                         // Line 1
                        (uint16_t) (pio_encode_pull(false, true) | pio_encode_sideset(5, 0)),                                           // Line 2
                        (uint16_t) (pio_encode_mov(pio_y, pio_osr) | pio_encode_sideset(5, 0)),                                         // Line 3
                        (uint16_t) (pio_encode_jmp_x_dec(7) | pio_encode_sideset(5, 0)),                                                // Line 4 (This should never advance to Line 5!!!)
                        (uint16_t) (pio_encode_jmp_x_dec(10) | pio_encode_sideset(5, 0)),                                               // Line 5
                        // TODO: Add sync point here
                        (uint16_t) (pio_encode_jmp(0) | pio_encode_sideset(5, 0)),                                                      // Line 6
                        (uint16_t) (pio_encode_jmp_y_dec(9) | pio_encode_sideset(5, 0)),                                                // Line 7
                        (uint16_t) (pio_encode_jmp(5) | pio_encode_sideset(5, (1 << (clk - ADDR_A)) | (1 << (data - ADDR_A)))),         // Line 8
                        (uint16_t) (pio_encode_jmp(4) | pio_encode_sideset(5, (1 << (clk - ADDR_A)))),                                  // Line 9
                        (uint16_t) (pio_encode_jmp(5) | pio_encode_sideset(5, (1 << (clk - ADDR_A)))),                                  // Line 10
                    };
                    static const struct pio_program pio_programs = {
                        .instructions = instructions,
                        .length = count_of(instructions),
                        .origin = 0,
                    };
                    pio_add_program(pio1, &pio_programs);
                    pio_sm_set_consecutive_pindirs(pio1, 0, Multiplex::HUB75::HUB75_ADDR_BASE, 5, true);
                
                    // Verify Serial Clock
                    constexpr float x = 125000000.0 / (multiplex_clock * 2.0);     // Someday this two will be a four.
                    static_assert(x >= 1.0, "Unabled to configure PIO for MULTIPLEX_CLOCK");

                    // PMP / SM
                    pio1->sm[0].clkdiv = ((uint32_t) floor(x) << PIO_SM0_CLKDIV_INT_LSB) | ((uint32_t) round((x - floor(x)) * 255.0) << PIO_SM0_CLKDIV_FRAC_LSB);
                    pio1->sm[0].pinctrl = (5 << PIO_SM0_PINCTRL_SIDESET_COUNT_LSB) | (Multiplex::HUB75::HUB75_ADDR_BASE << PIO_SM0_PINCTRL_SIDESET_BASE_LSB);
                    pio1->sm[0].shiftctrl = (1 << PIO_SM0_SHIFTCTRL_AUTOPULL_LSB) | (8 << PIO_SM0_SHIFTCTRL_PULL_THRESH_LSB);
                    pio1->sm[0].execctrl = (11 << PIO_SM0_EXECCTRL_WRAP_TOP_LSB);
                    pio1->sm[0].instr = pio_encode_jmp(0);
                    hw_set_bits(&pio1->ctrl, 1 << PIO_CTRL_SM_ENABLE_LSB);
                    pio_sm_claim(pio1, 0);
                }
                break;
            default:
                break;
        }
    }
}
