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
#include "Matrix/HUB75/BCM/memory_format.h"
#include "Matrix/helper.h"
#include "Matrix/HUB75/BCM/BCM_Worker.h"

namespace Matrix::Worker {
    Matrix::Buffer buf[Serial::num_framebuffers];
    static uint8_t bank = 0;
    static volatile uint8_t bank_vsync = 0;
    static volatile bool vsync = false;

    template <typename T> BCM_Worker<T>::BCM_Worker() {
        for (uint32_t i = 0; i < sizeof(index_table_t::v) / sizeof(uint32_t); i++)
            index_table.v[i] = 0;
        
        build_index_table();
    }

    template <typename T> inline void BCM_Worker<T>::build_index_table() {
        for (uint32_t i = 0; i < 16; i++)
            for (uint32_t j = 0; j < 4; j++)
                for (uint8_t k = 0; k < 6; k++)
                    if (i & (1 << j))
                        index_table.table[i][k][j / sizeof(T)] |= 1 << (k + ((j % sizeof(T)) * 8));
    }

    template <typename T> inline void BCM_Worker<T>::process_packet(BCM_Buffer *buffer, Serial::packet *p) {
        for (uint8_t y = 0; y < MULTIPLEX; y++) {
            for (uint16_t x = 0; x < COLUMNS; x++) {
                set_pixel(buffer, x, y, p->data[y][x].get_red(), p->data[y][x].get_green(), p->data[y][x].get_blue(), 
                    p->data[y + MULTIPLEX][x].get_red(), p->data[y + MULTIPLEX][x].get_green(), p->data[y + MULTIPLEX][x].get_blue());
            }
        }

        while (vsync) {
            // Block
        }

        vsync = true;
        bank = (bank + 1) % Serial::num_framebuffers;
    }

    template <typename T> inline T *BCM_Worker<T>::get_table(uint16_t v, uint8_t i, uint8_t nibble) {
        constexpr uint32_t div = std::max((uint32_t) Serial::range_high / 1 << PWM_bits, (uint32_t) 1);
        constexpr uint32_t mul = std::max((uint32_t) 1 << PWM_bits / Serial::range_high, (uint32_t) 1);
        
        v = v * mul / div;
        //v %= (1 << PWM_bits);
        return index_table.table[(v >> (nibble * sizeof(T))) & ((1 << sizeof(T)) - 1)][i];
    }

    // Tricks: (Branch is index into vector via PC)
    //  1. Use types (read, write)
    //  2. Remove if with calculations (multiply and accumulate)
    //      2.1 SIMD may help
    //      2.2 Matrix operations may help
    //  3. Remove if with LUT (needs good cache)
    //      3.1 Matrix operations may help
    template <typename T> inline void BCM_Worker<T>::set_pixel(BCM_Buffer *buffer, uint8_t x, uint8_t y, uint16_t r0, uint16_t g0, uint16_t b0, uint16_t r1, uint16_t g1, uint16_t b1) {    
        for (uint32_t nib = 0; nib < PWM_bits; nib += sizeof(T)) {
            T *c[6] = { get_table(r0, 0, nib), get_table(g0, 1, nib), get_table(b0, 2, nib), get_table(r1, 3, nib), get_table(g1, 4, nib), get_table(b1, 5, nib) };
        
            // Superscalar Operation (forgive the loads)
            T p = *c[0] | *c[1] | *c[2] | *c[3] | *c[4] | *c[5];

            // Hopefully the compiler will sort this out. (Inlining set_value)
            for (uint32_t j = 0; j < sizeof(T); j++)
                buffer->set_value(y, (nib * sizeof(T)) + j, x + 1, (p >> (j * 8)) & 0xFF);
        }
    }

    template <typename T> inline void BCM_Worker<T>::save_buffer(BCM_Buffer *buffer, Serial::packet *p) {
        for (uint8_t y = 0; y < MULTIPLEX; y++) {
            for (uint32_t i = 0; i < PWM_bits; i++) {
                uint8_t *p0 = buffer->get_line(y, i);
                uint8_t *p1 = p->raw + (((y * PWM_bits) + i) * Matrix::Buffer::get_line_length());

                for (uint8_t x = 1; x < Matrix::Buffer::get_line_length(); x++) {
                    p0[x] = p1[x];
                }
            }
        }

        while (vsync) {
            // Block
        }

        vsync = true;
        bank = (bank + 1) % Serial::num_framebuffers;
    }    

    template <typename T> inline void BCM_Worker<T>::save_buffer(BCM_Buffer *buffer, BCM_Buffer *p) {
        for (uint8_t y = 0; y < MULTIPLEX; y++) {
            for (uint32_t i = 0; i < (1 << PWM_bits); i++) {
                uint8_t *p0 = buffer->get_line(y, i);
                uint8_t *p1 = p->get_line(y, i);

                for (uint8_t x = 1; x < Matrix::Buffer::get_line_length(); x++) {
                    p0[x] = p1[x];
                }
            }
        }

        while (vsync) {
            // Block
        }

        vsync = true;
        bank = (bank + 1) % Serial::num_framebuffers;
    } 
    
    template <typename T> inline static void worker_internal() {
        static BCM_Worker<T> w;
        
        while(1) {
            switch (APP::multicore_fifo_pop_blocking_inline()) {
                case 0:
                    {
                        Serial::packet *p = (Serial::packet *) APP::multicore_fifo_pop_blocking_inline();
                        w.process_packet(p);
                    }
                    break;
                case 1:
                    {
                        Serial::packet *p = (Serial::packet *) APP::multicore_fifo_pop_blocking_inline();
                        w.save_buffer(p);
                    }
                    break;
                case 2:
                    {
                        Matrix::Buffer *p = (Matrix::Buffer *) APP::multicore_fifo_pop_blocking_inline();
                        w.save_buffer(p);
                    }
                    break;
                default:
                    break;
            }
        }
    }

    void work() {
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

    void __not_in_flash_func(process)(Serial::packet *buffer, bool isBuffer) {
        if (!isBuffer) {
            APP::multicore_fifo_push_blocking_inline(0);
            APP::multicore_fifo_push_blocking_inline((uint32_t) buffer);
        }
        else {
            APP::multicore_fifo_push_blocking_inline(1);
            APP::multicore_fifo_push_blocking_inline((uint32_t) buffer);
        }
    }

    void __not_in_flash_func(process)(Matrix::Buffer *buffer) {
        APP::multicore_fifo_push_blocking_inline(1);
        APP::multicore_fifo_push_blocking_inline((uint32_t) buffer);
    }

    Matrix::Buffer *__not_in_flash_func(get_front_buffer)() {
        Matrix::Buffer *result = nullptr;

        if (vsync) {
            result = &buf[bank_vsync];
            bank_vsync = (bank_vsync + 1) % Serial::num_framebuffers;
            vsync = false;
        }

        return result;
    }

    Matrix::Buffer *__not_in_flash_func(get_front_buffer)(uint8_t *id) {
        Matrix::Buffer *result = nullptr;
        if (id != nullptr && vsync) {
            result = &buf[bank_vsync];
            *id = bank_vsync;
            bank_vsync = (bank_vsync + 1) % Serial::num_framebuffers;
            vsync = false;
        }
        return result;
    }
}
