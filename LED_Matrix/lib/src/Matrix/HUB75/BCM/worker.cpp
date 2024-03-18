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
#include "Matrix/HUB75/BCM/memory_format.h"
#include "Matrix/helper.h"
#include "Matrix/HUB75/BCM/BCM_worker.h"

namespace Matrix::Worker {
    test2 buf[Serial::num_framebuffers];
    static uint8_t bank = 0;

    template <typename T> BCM_worker<T>::BCM_worker() {
        for (uint32_t i = 0; i < sizeof(index_table_t::v) / sizeof(uint32_t); i++)
            index_table.v[i] = 0;
        
        build_index_table();
    }

    template <typename T> void BCM_worker<T>::build_index_table() {
        for (uint32_t i = 0; i < 16; i++)
            for (uint32_t j = 0; j < 4; j++)
                for (uint8_t k = 0; k < 6; k++)
                    if (i & (1 << j))
                        index_table.table[i][k][j / sizeof(T)] |= 1 << (k + ((j % sizeof(T)) * 8));
    }

    template <typename T> void __not_in_flash_func(BCM_worker<T>::process_packet)(Serial::packet *p) {
        for (uint8_t y = 0; y < MULTIPLEX; y++) {
            for (uint16_t x = 0; x < COLUMNS; x++) {
                set_pixel(x, y, p->data[y][x].red, p->data[y][x].green, p->data[y][x].blue, p->data[y + MULTIPLEX][x].red, p->data[y + MULTIPLEX][x].green, p->data[y + MULTIPLEX][x].blue);
            }
        }

        APP::multicore_fifo_push_blocking_inline(bank);
        bank = (bank + 1) % Serial::num_framebuffers;
    }

    template <typename T> T *__not_in_flash_func(BCM_worker<T>::get_table)(uint16_t v, uint8_t i, uint8_t nibble) {
        constexpr uint32_t div = std::max((uint32_t) Serial::range_high / 1 << PWM_bits, (uint32_t) 1);
        constexpr uint32_t mul = std::max((uint32_t) 1 << PWM_bits / Serial::range_high, (uint32_t) 1);
        
        v = v * mul / div;
        //v %= (1 << PWM_bits);
        return index_table.table[(v >> (nibble * sizeof(T))) & ((1 << sizeof(T)) - 1)][i];
    }

    template <typename T> void __not_in_flash_func(BCM_worker<T>::set_pixel)(uint8_t x, uint8_t y, uint16_t r0, uint16_t g0, uint16_t b0, uint16_t r1, uint16_t g1, uint16_t b1) {    
        for (uint32_t nib = 0; nib < PWM_bits; nib += sizeof(T)) {
            T *c[6] = { get_table(r0, 0, nib), get_table(g0, 1, nib), get_table(b0, 2, nib), get_table(r1, 3, nib), get_table(g1, 4, nib), get_table(b1, 5, nib) };
        
            T p = *c[0] + *c[1] + *c[2] + *c[3] + *c[4] + *c[5];

            for (uint32_t j = 0; j < sizeof(T); j++)
                buf[bank][y][(nib * sizeof(T)) + j][x + 1] = (p >> (j * 8)) & 0xFF;
        }
    }

    template <typename T> static void __not_in_flash_func(worker_internal)() {
        static BCM_worker<T> w;
        
        while(1) {
            Serial::packet *p = (Serial::packet *) APP::multicore_fifo_pop_blocking_inline();
            w.process_packet(p);
        }
    }

    void __not_in_flash_func(work)() {
        // Compiler should remove all but one of these.
        switch (PWM_bits % 4) {
            case 0:
                worker_internal<uint32_t>();
                break;
            case 2:
                worker_internal<uint16_t>();
                break;
            default:
                worker_internal<uint8_t>();
                break;
        }
    }

    void __not_in_flash_func(process)(void *arg) {
        APP::multicore_fifo_push_blocking_inline((uint32_t) arg);
    }

    void *__not_in_flash_func(get_front_buffer)() {
        if (multicore_fifo_rvalid()) {
            uint32_t i = (uint32_t) APP::multicore_fifo_pop_blocking_inline();
            return (void *) &buf[i % Serial::num_framebuffers];
        }

        return nullptr;
    }

    uint32_t __not_in_flash_func(get_buffer_size)() {
        return Loafer::get_buffer_size();
    }
}
