/* 
 * File:   PWM_Matrix.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "Matrix/HUB75/PWM/PWM_Matrix.h"
#include "Matrix/HUB75/PWM/PWM_Packet.h"
#include "Matrix/HUB75/PWM/PWM_Buffer.h"
#include "Matrix/HUB75/PWM/PWM_Calculator.h"
#include "Matrix/HUB75/hw_config.h"
#include "SIMD/SIMD_QUARTER.h"
#include "SIMD/SIMD_SINGLE.h"

namespace Matrix {
    template <typename T, typename R, typename W> PWM_Matrix<T, R, W>::PWM_Matrix(uint8_t scan, uint8_t pwm_bits, uint8_t columns) {
        _scan = scan;
        _columns = columns;
        _pwm_bits = pwm_bits;
        _worker = PWM_Worker<T, R, W>::get_worker(scan, 1 << pwm_bits, columns);
    }

    template <typename T, typename R, typename W> PWM_Matrix<T, R, W> *PWM_Matrix<T, R, W>::create_matrix(uint8_t scan, uint8_t pwm_bits, uint8_t columns) {
        if (verify_configuration()) {
            // TODO: Create singleton?
        }

        return nullptr;
    }

    template <typename T, typename R, typename W> PWM_Matrix<T, R, W>::~PWM_Matrix() {
        delete _worker;
    }

    template <typename T, typename R, typename W> void PWM_Matrix<T, R, W>::show(unique_ptr<Buffer<T>> &buffer) {
        unique_ptr<Buffer<T>> p(get_buffer());
        buffer.swap(p);
        _worker->convert(p.release());
    }
    
    template <typename T, typename R, typename W> void PWM_Matrix<T, R, W>::show(unique_ptr<Packet<R>> &packet) {
        unique_ptr<Packet<R>> p(get_packet());
        packet.swap(p);
        _worker->convert(p.release());
    }
    
    template <typename T, typename R, typename W> unique_ptr<Buffer<T>> PWM_Matrix<T, R, W>::get_buffer() {
        uint32_t i = ((sizeof(R) * 8) / 6) * 2;
        unique_ptr<Buffer<T>> result(PWM_Buffer<T>::create_pwm_buffer(_scan * i, _columns));
        return result;
    }
    
    template <typename T, typename R, typename W> unique_ptr<Packet<R>> PWM_Matrix<T, R, W>::get_packet() {
        unique_ptr<PWM_Packet<R>> result(PWM_Packet<R>::create_pwm_packet(_scan, 1 << _pwm_bits, _columns));
        return result;
    }

    template class PWM_Matrix<RGB24, HUB75_UNIT, SIMD::SIMD_UNIT<HUB75_UNIT>>;
    template class PWM_Matrix<RGB48, HUB75_UNIT, SIMD::SIMD_UNIT<HUB75_UNIT>>;
    template class PWM_Matrix<RGB_222, HUB75_UNIT, SIMD::SIMD_UNIT<HUB75_UNIT>>;
    template class PWM_Matrix<RGB_555, HUB75_UNIT, SIMD::SIMD_UNIT<HUB75_UNIT>>;
}
