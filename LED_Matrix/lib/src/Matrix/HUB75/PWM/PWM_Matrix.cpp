/* 
 * File:   PWM_Matrix.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "Matrix/HUB75/PWM/PWM_Matrix.h"
#include "Matrix/HUB75/PWM/PWM_Packet.h"
#include "Matrix/HUB75/PWM/PWM_Calculator.h"

namespace Matrix {
    template <typename T, typename R> PWM_Matrix<T, R>::PWM_Matrix(uint8_t scan, uint8_t pwm_bits, uint8_t columns) : Matrix<T,R>() {
        _scan = scan;
        _columns = columns;
        _pwm_bits = pwm_bits;
        _worker = new PWM_Worker<T, R>(scan, 1 << pwm_bits, columns);
    }

    template <typename T, typename R> PWM_Matrix<T, R> *PWM_Matrix<T, R>::create_matrix(uint8_t scan, uint8_t pwm_bits, uint8_t columns) {
        if (verify_configuration()) {
            // TODO: Create singleton?
        }
    }

    template <typename T, typename R> PWM_Matrix<T, R>::~PWM_Matrix() {
        delete _worker;
    }

    template <typename T, typename R> void PWM_Matrix<T, R>::show(unique_ptr<Buffer<T>> &buffer) {
        unique_ptr<Buffer<T>> p(get_buffer());
        buffer.swap(p);
        _worker->convert(p.release(), true);
    }
    
    template <typename T, typename R> void PWM_Matrix<T, R>::show(unique_ptr<Packet<R>> &packet) {
        unique_ptr<Packet<R>> p(get_packet());
        packet.swap(p);
        _worker->convert(p.release(), true);
    }
    
    template <typename T, typename R> unique_ptr<Buffer<T>> PWM_Matrix<T, R>::get_buffer() {
        unique_ptr<Buffer<T>> result(nullptr);  // TODO:
        return result;
    }
    
    template <typename T, typename R> unique_ptr<Packet<R>> PWM_Matrix<T, R>::get_packet() {
        unique_ptr<PWM_Packet<R>> result(PWM_Packet<R>::create_pwm_packet(_scan, 1 << _pwm_bits, _columns));
        return result;
    }
}
