/* 
 * File:   Shifter.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "Multiplex/Multiplex.h"
#include "Multiplex/config.h"
#include "hardware/gpio.h"

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
        }
        else {
            // TODO:
            //  Add PIO shifter module
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
