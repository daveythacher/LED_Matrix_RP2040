/* 
 * File:   worker.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#include <algorithm>
#include <atomic>
#include "Matrix/HUB75/PWM/PWM_Worker.h"
#include "Matrix/HUB75/PWM/PWM_Multiplex.h"
#include "Matrix/HUB75/hw_config.h"
#include "SIMD/SIMD_QUARTER.h"
#include "SIMD/SIMD_SINGLE.h"

namespace Matrix {
    template <typename T, typename R, typename W> PWM_Worker<T, R, W>::PWM_Worker(uint8_t scan, uint16_t steps, uint8_t columns) {
        _scan = scan;
        _steps = steps;
        _columns = columns;
        _width = (sizeof(R) * 8 / 6) * 6; // TODO: HUB75 hardware configuration
        _size = std::max((_steps / W::size()), (uint32_t) 1);
        _index_table = new W[_width * _size * _steps];
        _multiplex = new PWM_Multiplex<R>();
        _thread[0] = new Concurrent::Thread(work, 4096, 1, this);
        _thread[1] = new Concurrent::Thread(work, 4096, 1, this);
        _queue = nullptr; // TODO: Updates
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

    template <typename T, typename R, typename W> PWM_Worker<T, R, W>::~PWM_Worker() {
        delete[] _index_table;
        delete _multiplex;
        delete _thread;
        delete _queue;
        delete _mutex;
    }


    template <typename T, typename R, typename W> void PWM_Worker<T, R, W>::convert(Packet<R> *packet) {
        _mutex->lock();

        // Force a resync of order by waiting for Buffer pipeline to stall completely.
        //  This is not the fastest way, but mixing these is not intended.
        //  This avoids fragmentation.
        while (_queue->available() || !_idle) { // TODO: Consider a glitch or hazard here
            Concurrent::Thread::Yield();
        }

        _multiplex->show(static_cast<PWM_Packet<R> *>(packet));
        _mutex->unlock();
    }

    template <typename T, typename R, typename W> void PWM_Worker<T, R, W>::convert(Buffer<T> *buffer) {
        _mutex->lock();

        // TODO: Push onto local queue

        _mutex->unlock();
    }

    template <typename T, typename R, typename W> inline W *PWM_Worker<T, R, W>::get_table(uint16_t v, uint8_t i) {
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
    template <typename T, typename R, typename W> inline void PWM_Worker<T, R, W>::set_pixel(R *val, T *pixel, uint8_t index) {    
        W *c[3] = { get_table(pixel->get_red(), index + 0), get_table(pixel->get_green(), index + 1), get_table(pixel->get_blue(), index + 2) };
    
        for (uint32_t i = 0; i < _steps; i += W::size()) {
            // Superscalar Operation (forgive the loads)
            W p = *c[0] | *c[1] | *c[2];

            // Hopefully the compiler will sort this out. (Inlining set_value)
            for (uint32_t j = 0; (j < W::size()) && ((i + j) < _steps); j++) {
                *val |= p.get(j);
                val++;
            }

            // Superscalar operation
            for (uint32_t j = 0; j < 3; j++)
                ++c[j];
        }
    }

    template <typename T, typename R, typename W> inline void PWM_Worker<T, R, W>::build_index_table() {
        uint8_t size = W::size();

        for (uint32_t i = 0; i < _steps; i++) {
            for (uint32_t j = 0; j < _width; j++) {
                for (uint32_t k = 0; k < i; k++) {
                    _index_table[(i * (_width * _size)) + (j * _size) + k / size].set(1 << j, k % size);
                }
            }
        }
    }

    template <typename T, typename R, typename W> uint8_t PWM_Worker<T, R, W>::get_thread_id() {
        static std::atomic<uint8_t> _thread_id = 0;
        ++_thread_id;
        return _thread_id - 1;
    }

    template <typename T, typename R, typename W> void PWM_Worker<T, R, W>::work(void *arg) {
        PWM_Worker<T, R, W> *object = static_cast<PWM_Worker<T, R, W> *>(arg);
        uint8_t id = object->get_thread_id();

        while (1) {
            object->_idle = true;

            if (id == 0) {
                // TODO: Parent thread
            }
            else {
                // TODO: Child thread
            }
        }
    }

    template class PWM_Worker<RGB24, HUB75_UNIT, SIMD::SIMD_UNIT<HUB75_UNIT>>;
    template class PWM_Worker<RGB48, HUB75_UNIT, SIMD::SIMD_UNIT<HUB75_UNIT>>;
    template class PWM_Worker<RGB_222, HUB75_UNIT, SIMD::SIMD_UNIT<HUB75_UNIT>>;
    template class PWM_Worker<RGB_555, HUB75_UNIT, SIMD::SIMD_UNIT<HUB75_UNIT>>;
}
