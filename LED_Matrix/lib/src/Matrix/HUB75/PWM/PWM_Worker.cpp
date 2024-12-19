/* 
 * File:   worker.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#include <algorithm>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "pico/multicore.h"
#include "Matrix/HUB75/PWM/PWM_Worker.h"

namespace Matrix {
    Matrix::Buffer buf[Serial::num_framebuffers];
    static uint8_t bank = 0;
    static volatile uint8_t bank_vsync = 0;
    static volatile bool vsync = false;

    template <typename T, typename R> PWM_Worker<T, R>::PWM_Worker(uint8_t scan, uint16_t steps, uint8_t columns) {
        _scan = scan;
        _steps = steps;
        _columns = columns;
        _width = sizeof(R) * 8;
        _size = std::max((_steps / SIMD::SIMD_QUARTER<R>::size()), (uint32_t) 1); // TODO: Make SIMD::SIMD_QUARTER a template argument
        _index_table = new SIMD::SIMD_QUARTER<R>[sizeof(R) * 8 * _size * _steps];
        _multiplex = new PWM_Multiplex<T, R>();
        _thread = new Concurrent::Thread(work, 4096, 1, this);
        _queue = nullptr; // TODO: Updates
        _mutex = new Concurrent::Mutex();
        

        for (uint32_t i = 0; i < _steps; i++) {
            for (uint32_t j = 0; j < _width; j++) {
                for (uint32_t k = 0; k < _size; k++) {
                    for (uint32_t l = 0; l < SIMD::SIMD_QUARTER<R>::size(); l++) {
                        index_table[(i * (_width * _size)) + (j * _size) + k].set(0, l);
                    }
                }
            }
        }

        build_index_table();
    }

    template <typename T, typename R> PWM_Worker<T, R>::~PWM_Worker() {
        delete[] _index_table;
        delete _multiplex;
        delete _thread;
        delete _queue;
        delete _mutex;
    }

    template <typename T, typename R> inline SIMD::SIMD_QUARTER<R> *PWM_Worker<T, R>::get_table(uint16_t v, uint8_t i) {
        uint32_t div = std::max((uint32_t) T::range_high / _steps, (uint32_t) 1);
        uint32_t mul = std::max((uint32_t) _steps / T::range_high, (uint32_t) 1);

        v = v * mul / div;
        return &index_table[(v * (_width * _size)) + (i * _size)];
    }

    // Tricks: (Branch is index into vector via PC)
    //  1. Use types
    //  2. Remove if with calculations (multiply and accumulate)
    //      2.1 SIMD may help
    //      2.2 Matrix operations may help
    //  3. Remove if with LUT (needs good cache)
    //      3.1 Matrix operations may help
    template <typename T> inline void PWM_Worker<T>::set_pixel(uint8_t x, uint8_t y, uint16_t r0, uint16_t g0, uint16_t b0, uint16_t r1, uint16_t g1, uint16_t b1) {    
        SIMD::SIMD_QUARTER<T> *c[6] = { get_table(r0, 0), get_table(g0, 1), get_table(b0, 2), get_table(r1, 3), get_table(g1, 4), get_table(b1, 5) };
    
        for (uint32_t i = 0; i < (1 << PWM_bits); i += SIMD::SIMD_QUARTER<T>::size()) {
            // Superscalar Operation (forgive the loads)
            SIMD::SIMD_QUARTER<T> p = *c[0] | *c[1] | *c[2] | *c[3] | *c[4] | *c[5];

            // Hopefully the compiler will sort this out. (Inlining set_value)
            for (uint32_t j = 0; (j < (SIMD::SIMD_QUARTER<T>::size())) && ((i + j) < (1 << PWM_bits)); j++)
                buf[bank].set_value(y, i + j, x + 1, p.v[j]);

            // Superscalar operation
            for (uint32_t j = 0; j < 6; j++)
                ++c[j];
        }
    }

    template <typename T, typename R> inline void PWM_Worker<T, R>::build_index_table() {
        uint8_t size = SIMD::SIMD_QUARTER<R>::size();

        for (uint32_t i = 0; i < _steps; i++) {
            for (uint32_t j = 0; j < _width; j++) {
                for (uint32_t k = 0; k < i; k++) {
                    index_table[(i * (_width * _size)) + (j * _size) + k / size].set(1 << j, k % size);
                }
            }
        }
    }

    template <typename T> inline void PWM_Worker<T>::process_packet(Serial::packet *p) {
        for (uint8_t y = 0; y < MULTIPLEX; y++) {
            for (uint16_t x = 0; x < COLUMNS; x++) {
                set_pixel(x, y, p->data[y][x].get_red(), p->data[y][x].get_green(), p->data[y][x].get_blue(), 
                    p->data[y + MULTIPLEX][x].get_red(), p->data[y + MULTIPLEX][x].get_green(), p->data[y + MULTIPLEX][x].get_blue());            }
        }

        while (vsync) {
            // Block
        }

        vsync = true;
        bank = (bank + 1) % Serial::num_framebuffers;
    }   

    template <typename T> inline void PWM_Worker<T>::save_buffer(Serial::packet *p) {
        for (uint8_t y = 0; y < MULTIPLEX; y++) {
            for (uint32_t i = 0; i < (1 << PWM_bits); i++) {
                uint8_t *p0 = buf[bank].get_line(y, i);
                uint8_t *p1 = p->raw + (((y * (1 << PWM_bits)) + i) * Matrix::Buffer::get_line_length());

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

    template <typename T> inline void PWM_Worker<T>::save_buffer(Matrix::Buffer *p) {
        for (uint8_t y = 0; y < MULTIPLEX; y++) {
            for (uint32_t i = 0; i < (1 << PWM_bits); i++) {
                uint8_t *p0 = buf[bank].get_line(y, i);
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

    template <typename T, typename R> void PWM_Worker<T, R>::work(void *arg) {
        PWM_Worker<T, R> *object = static_cast<PWM_Worker<T, R> *>(arg);

        while (1) {
            // TODO: Update
        }
    }
}
