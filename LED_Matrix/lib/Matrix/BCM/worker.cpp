/* 
 * File:   worker.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <algorithm>
#include "pico/multicore.h"
#include "Serial/config.h"
#include "Matrix/matrix.h"
#include "Matrix/BCM/memory_format.h"
#include "Matrix/helper.h"

namespace Matrix {
    extern test2 buf[];
}

namespace Matrix::Worker {
    static uint8_t bank = 1;
    volatile bool vsync = false;

    union index_table_t {
        uint8_t table[1 << PWM_bits][6][PWM_bits];
        uint32_t v[(1 << PWM_bits) * 6 * PWM_bits / 4];
    };

    static index_table_t index_table;

    static uint8_t *__not_in_flash_func(get_table)(uint16_t v, uint8_t i) {
        constexpr uint32_t div = std::max((uint32_t) Serial::range_high / 1 << PWM_bits, (uint32_t) 1);
        constexpr uint32_t mul = std::max((uint32_t) 1 << PWM_bits / Serial::range_high, (uint32_t) 1);
        
        v = v * mul / div;
        //v %= (1 << PWM_bits);
        return index_table.table[v][i];
    }

    // Forgive the shameless and reckless casting.
    template <typename T> static void __not_in_flash_func(set_pixel)(uint8_t x, uint8_t y, uint16_t r0, uint16_t g0, uint16_t b0, uint16_t r1, uint16_t g1, uint16_t b1) {    
        T *c[6] = { (T *) get_table(r0, 0), (T *) get_table(g0, 1), (T *) get_table(b0, 2), (T *) get_table(r1, 3), (T *) get_table(g1, 4), (T *) get_table(b1, 5) };
    
        for (uint32_t i = 0; i < PWM_bits; i += sizeof(T)) {
            T p = *c[0] + *c[1] + *c[2] + *c[3] + *c[4] + *c[5];

            for (uint32_t j = 0; (j < sizeof(T)) && ((i + j) < PWM_bits); j++)
                Matrix::buf[bank][y][i + j][x + 1] = (p >> (j * 8)) & 0xFF;

            for (uint32_t j = 0; j < 6; j++)
                ++c[j];
        }
    }

    static void build_index_table() {
        for (uint32_t i = 0; i < (1 << PWM_bits); i++)
            for (uint32_t j = 0; j < PWM_bits; j++)
                for (uint8_t k = 0; k < 6; k++)
                    if (i & (1 << j))
                        index_table.table[i][k][j] = 1 << k;
                    else
                        index_table.table[i][k][j] = 0;
    }

    static void __not_in_flash_func(process_packet)(Serial::packet *p) {
        for (uint8_t y = 0; y < MULTIPLEX; y++) {
            for (uint16_t x = 0; x < COLUMNS; x++) {

                // Compiler should remove all but one of these.
                switch (PWM_bits % 4) {
                    case 0:
                        set_pixel<uint32_t>(x, y, p->data[y][x].red, p->data[y][x].green, p->data[y][x].blue, p->data[y + MULTIPLEX][x].red, p->data[y + MULTIPLEX][x].green, p->data[y + MULTIPLEX][x].blue);
                        break;
                    case 2:
                        set_pixel<uint16_t>(x, y, p->data[y][x].red, p->data[y][x].green, p->data[y][x].blue, p->data[y + MULTIPLEX][x].red, p->data[y + MULTIPLEX][x].green, p->data[y + MULTIPLEX][x].blue);
                        break;
                    default:
                        set_pixel<uint8_t>(x, y, p->data[y][x].red, p->data[y][x].green, p->data[y][x].blue, p->data[y + MULTIPLEX][x].red, p->data[y + MULTIPLEX][x].green, p->data[y + MULTIPLEX][x].blue);
                        break;
                }
            }
        }
        
        if (!vsync) {
            bank = (bank + 1) % Serial::num_framebuffers;
            vsync = true;
        }
    }

    void __not_in_flash_func(work)() {
        build_index_table();
        
        while(1) {
            Serial::packet *p = (Serial::packet *) APP::multicore_fifo_pop_blocking_inline();
            process_packet(p);
        }
    }

    bool __not_in_flash_func(process)(void *arg, bool block) {
        if (multicore_fifo_wready() || block) {
            APP::multicore_fifo_push_blocking_inline((uint32_t) arg);
            return true;
        }

        return false;
    }
}
