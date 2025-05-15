/* 
 * File:   PWM_Worker_LUT.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#include <algorithm>
#include <atomic>
#include "Matrix/Mono/PWM/PWM_Worker/LUT/PWM_Worker_LUT.h"
#include "Matrix/Mono/PWM/PWM_Multiplex.h"
#include "Matrix/Mono/hw_config.h"
#include "SIMD/SIMD.h"

namespace Matrix {
    PWM_Worker_LUT::PWM_Worker_LUT(uint8_t scan, uint16_t steps, uint8_t columns) : PWM_Worker::PWM_Worker() {
        _scan = scan;
        _steps = steps;
        _columns = columns;
        _width = (sizeof(R) * 8 / 6) * 6; // TODO: HUB75 hardware configuration
        _size = std::max((_steps / W::size()), (uint32_t) 1);
        _index_table = new W[_width * _size * _steps];
        _multiplex = new PWM_Multiplex<R>();
        _thread = new Concurrent::Thread(work, 4096, 1, this);
        _queue = new Concurrent::Queue<Packet *>(2);
        _mutex = new Concurrent::Mutex();
        

        for (uint32_t i = 0; i < _steps; i++) {
            for (uint32_t j = 0; j < _width; j++) {
                for (uint32_t k = 0; k < _size; k++) {
                    for (uint32_t l = 0; l < W::size(); l++) {
                        _index_table[(i * (_width * _size)) + (j * _size) + k].set(0, l);
                    }
                }
            }
        }

        build_index_table();
    }

    PWM_Worker_LUT::~PWM_Worker_LUT() {
        delete[] _index_table;
        delete _multiplex;
        delete _thread;
        delete _queue;
        delete _mutex;
    }

    void PWM_Worker_LUT::convert(Packet *packet) {
        // This should be a sync lock
        _mutex->lock();

        while (_queue->available()) {
            Concurrent::Thread::Yield();
        }

        _queue->push(packet);

        // We have a polymorphic problem here. (We demoted it down and now we have to promote it.)
        //  Fact of the matter, I guess.
        _multiplex->show(static_cast<PWM_Packet *>(packet));

        // Break out of the proceedure
        _mutex->unlock();
    }

    inline W *PWM_Worker_LUT::get_table(uint16_t v, uint8_t i) {
        uint32_t div = std::max((uint32_t) T::range_high / _steps, (uint32_t) 1);
        uint32_t mul = std::max((uint32_t) _steps / T::range_high, (uint32_t) 1);

        v = v * mul / div;
        return &_index_table[(v * (_width * _size)) + (i * _size)];
    }

    // Tricks: (Branch is index into vector via PC)
    //  1. Use types
    //  2. Remove if with calculations (multiply and accumulate)
    //      2.1 SIMD may help
    //      2.2 Matrix operations may help
    //  3. Remove if with LUT (needs good cache)
    //      3.1 Matrix operations may help
    inline void PWM_Worker_LUT::set_pixel(X *val, R *pixel, uint8_t index, uint8_t shift) {    
        W *c[3] = { get_table(pixel->get_red(), index + 0), get_table(pixel->get_green(), index + 1), get_table(pixel->get_blue(), index + 2) };
    
        for (uint32_t i = 0; i < _steps; i += W::size()) {
            // Superscalar Operation (forgive the loads)
            //  Remember c is a vector of two or four elements being or'd in parallel.
            //  We do not do matrix operations so the three operations are in serial.
            W p = *c[0] | *c[1] | *c[2];

            // Hopefully the compiler will sort this out. (Inlining set_value)
            for (uint32_t j = 0; (j < W::size()) && ((i + j) < _steps); j++) {
                *val |= p.get(j) << shift;
                val++;
            }

            // Superscalar operation
            for (uint32_t j = 0; j < 3; j++)
                ++c[j];
        }
    }

    inline void PWM_Worker_LUT::build_index_table() {
        uint8_t size = W::size();

        for (uint32_t i = 0; i < _steps; i++) {
            for (uint32_t j = 0; j < _width; j++) {
                for (uint32_t k = 0; k < i; k++) {
                    _index_table[(i * (_width * _size)) + (j * _size) + k / size].set(1 << j, k % size);
                }
            }
        }
    }

    void PWM_Worker_LUT::work(void *arg) {
        PWM_Worker_LUT *object = static_cast<PWM_Worker_LUT *>(arg);

        while (1) {
            object->_idle = true;

            // TODO:
        }
    }
}
