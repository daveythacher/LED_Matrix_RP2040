/* 
 * File:   PWM_Matrix.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "Matrix/HUB75/PWM/PWM_Matrix.h"
#include "Matrix/HUB75/PWM/PWM_Buffer.h"
#include "Matrix/HUB75/PWM/PWM_Calculator.h"

namespace Matrix {
    template <typename T, typename R> PWM_Matrix<T, R>::PWM_Matrix(uint8_t scan, uint8_t columns) : Matrix<T,R>() {
        _scan = scan;
        _columns = columns;
        _worker = new PWM_Worker<T, R>();
    }

    template <typename T, typename R> PWM_Matrix<T, R> *PWM_Matrix<T, R>::create_matrix(uint8_t scan, uint8_t columns) {
        if (verify_configuration()) {
            // TODO: Create singleton?
        }
    }

    template <typename T, typename R> PWM_Matrix<T, R>::~PWM_Matrix() {
        delete _worker;
    }

    template <typename T, typename R> void PWM_Matrix<T, R>::show(Buffer<T> *buffer) {
        _worker->convert(buffer, true);
    }
    
    template <typename T, typename R> void PWM_Matrix<T, R>::show(Packet<R> *buffer) {
        _worker->convert(buffer, true);
    }
    
    template <typename T, typename R> void PWM_Matrix<T, R>::show(Packet<R> *buffer) {
        _worker->convert(buffer, true);
    }
    
    template <typename T, typename R> unique_ptr<Buffer<T>> PWM_Matrix<T, R>::get_buffer() {
        unique_ptr<Buffer<T>> result(nullptr);  // TODO:
        return result;
    }
    
    template <typename T, typename R> unique_ptr<Packet<R>> PWM_Matrix<T, R>::get_packet() {
        unique_ptr<PWM_Buffer<R>> result(PWM_Buffer<R>::create_pwm_buffer(_scan, 1, _columns));
        return result;
    }
}
